#!/usr/bin/env python3
import sys
import os
import configparser
import re


def parse_value(s: str):
	s = s.strip()
	low = s.lower()
	if low in ("true", "false"):
		return True if low == "true" else False
	# try int (no dot)
	if re.fullmatch(r"[-+]?\d+", s):
		try:
			return int(s)
		except Exception:
			pass
	# try float
	try:
		return float(s)
	except Exception:
		return s


def fmt_cpp_bool(v: bool) -> str:
	return "true" if v else "false"


def fmt_cpp_float(v: float) -> str:
	# print as float literal with minimal trailing zeros, keep at least one decimal
	try:
		x = float(v)
	except Exception:
		x = v
	if not isinstance(x, float):
		return str(x)
	s = f"{x:.6f}"
	if "." in s:
		s = s.rstrip("0").rstrip(".")
	if "." not in s:
		s += ".0"
	return s + "f"


def fmt_cpp_raw_int_with_error(v: int) -> str:
	return f"{int(v)} /* conversion error */"


def make_c_ident(name: str) -> str:
	name = name.replace("&", "And")
	s = re.sub(r"[^0-9a-zA-Z]", "_", name)
	s = re.sub(r"_+", "_", s)
	s = s.strip("_")
	if not s:
		s = "Style"
	if re.match(r"^\d", s):
		s = "S" + s
	return s


def main():
	if len(sys.argv) < 2:
		print("Usage: python ini2style.py path/to/theme.ini", file=sys.stderr)
		sys.exit(2)

	path = sys.argv[1]
	if not os.path.isfile(path):
		print(f"File not found: {path}", file=sys.stderr)
		sys.exit(2)

	cfg = configparser.ConfigParser(interpolation=None)
	cfg.optionxform = str  # preserve case
	try:
		cfg.read(path, encoding='utf-8')
	except Exception as e:
		print(f"Failed to read INI: {e}", file=sys.stderr)
		sys.exit(2)

	if 'Theme' in cfg:
		section = cfg['Theme']
	else:
		# fall back to first section
		secs = cfg.sections()
		if not secs:
			print("No sections in INI", file=sys.stderr)
			sys.exit(2)
		section = cfg[secs[0]]

	# group keys
	vec2 = {}   # name -> { 'X': val, 'Y': val }
	vec4 = {}   # name -> { 'R': val, 'G': val, 'B': val, 'A': val }
	scalars = {}

	for k in section:
		v = parse_value(section[k])
		m = re.match(r"^(.*)([XY])$", k)
		if m:
			name = m.group(1)
			axis = m.group(2)
			vec2.setdefault(name, {})[axis] = v
			continue
		m2 = re.match(r"^(.*)([RGBA])$", k)
		if m2 and k.startswith("Color"):
			name = m2.group(1)[len('Color'):]
			comp = m2.group(2)
			vec4.setdefault(name, {})[comp] = v
			continue
		# otherwise scalar
		scalars[k] = v

	# Build function name from filename
	base = os.path.splitext(os.path.basename(path))[0]
	fname = make_c_ident(base)
	func_name = f"ApplyStyle{fname}"

	out = []
	out.append(f"void {func_name}(ImGuiStyle* style)")
	out.append("{")

	# Scalars and vec2 (non-color) first
	# Merge vec2 into scalars for output if both components exist
	for name, comps in sorted(vec2.items()):
		if 'X' in comps and 'Y' in comps:
			x = comps['X']
			y = comps['Y']
			out.append(f"    style->{name} = ImVec2({fmt_cpp_float(x)}, {fmt_cpp_float(y)});")
		else:
			# output whichever exists
			if 'X' in comps:
				out.append(f"    style->{name}.x = {fmt_cpp_float(comps['X'])};")
			if 'Y' in comps:
				out.append(f"    style->{name}.y = {fmt_cpp_float(comps['Y'])};")

	# Enum/flags metadata
	DIR_FIELDS = {"WindowMenuButtonPosition", "ColorButtonPosition"}
	TREENODE_FLAGS_FIELDS = {"TreeLinesFlags"}
	HOVERED_FLAGS_FIELDS = {"HoverFlagsForTooltipMouse", "HoverFlagsForTooltipNav"}

	# ImGuiDir mapping
	IMGUI_DIR = {
		-1: "ImGuiDir_None",
		0: "ImGuiDir_Left",
		1: "ImGuiDir_Right",
		2: "ImGuiDir_Up",
		3: "ImGuiDir_Down",
	}

	# ImGuiTreeNodeFlags relevant to TreeLinesFlags
	TREENODE_SINGLETONS = {
		0: "ImGuiTreeNodeFlags_None",
		1 << 18: "ImGuiTreeNodeFlags_DrawLinesNone",
		1 << 19: "ImGuiTreeNodeFlags_DrawLinesFull",
		1 << 20: "ImGuiTreeNodeFlags_DrawLinesToNodes",
	}
	TREENODE_ALIASES = {
		# no additional aliases needed for TreeLinesFlags beyond singletons
	}

	# ImGuiHoveredFlags singletons
	HOVERED_SINGLETONS = {
		0: "ImGuiHoveredFlags_None",
		1 << 0: "ImGuiHoveredFlags_ChildWindows",
		1 << 1: "ImGuiHoveredFlags_RootWindow",
		1 << 2: "ImGuiHoveredFlags_AnyWindow",
		1 << 3: "ImGuiHoveredFlags_NoPopupHierarchy",
		1 << 5: "ImGuiHoveredFlags_AllowWhenBlockedByPopup",
		1 << 7: "ImGuiHoveredFlags_AllowWhenBlockedByActiveItem",
		1 << 8: "ImGuiHoveredFlags_AllowWhenOverlappedByItem",
		1 << 9: "ImGuiHoveredFlags_AllowWhenOverlappedByWindow",
		1 << 10: "ImGuiHoveredFlags_AllowWhenDisabled",
		1 << 11: "ImGuiHoveredFlags_NoNavOverride",
		1 << 12: "ImGuiHoveredFlags_ForTooltip",
		1 << 13: "ImGuiHoveredFlags_Stationary",
		1 << 14: "ImGuiHoveredFlags_DelayNone",
		1 << 15: "ImGuiHoveredFlags_DelayShort",
		1 << 16: "ImGuiHoveredFlags_DelayNormal",
		1 << 17: "ImGuiHoveredFlags_NoSharedDelay",
	}
	# Aliases composed of singletons
	HOVERED_ALIASES = {
		# AllowWhenOverlapped = AllowWhenOverlappedByItem | AllowWhenOverlappedByWindow
		"ImGuiHoveredFlags_AllowWhenOverlapped": (1 << 8) | (1 << 9),
		# RectOnly = AllowWhenBlockedByPopup | AllowWhenBlockedByActiveItem | AllowWhenOverlapped
		"ImGuiHoveredFlags_RectOnly": (1 << 5) | (1 << 7) | ((1 << 8) | (1 << 9)),
		# RootAndChildWindows = RootWindow | ChildWindows
		"ImGuiHoveredFlags_RootAndChildWindows": (1 << 1) | (1 << 0),
	}

	def popcount(n: int) -> int:
		c = 0
		x = int(n)
		while x:
			x &= x - 1
			c += 1
		return c

	def render_dir(val) -> str:
		try:
			ival = int(val)
		except Exception:
			return fmt_cpp_raw_int_with_error(val)
		return IMGUI_DIR.get(ival, fmt_cpp_raw_int_with_error(ival))

	def render_flags(value: int, singletons: dict, aliases: dict) -> str:
		try:
			v = int(value)
		except Exception:
			return fmt_cpp_raw_int_with_error(value)
		if v == 0 and 0 in singletons:
			return singletons[0]
		remaining = v
		parts = []
		# Greedy alias application by bit-count desc
		for name, aval in sorted(aliases.items(), key=lambda it: popcount(it[1]), reverse=True):
			if aval != 0 and (remaining & aval) == aval:
				parts.append(name)
				remaining &= ~aval
		# Then apply singletons (skip 0)
		for bit, name in sorted(singletons.items(), key=lambda it: it[0], reverse=True):
			if bit != 0 and (remaining & bit) == bit:
				parts.append(name)
				remaining &= ~bit
		if remaining != 0:
			return fmt_cpp_raw_int_with_error(v)
		if not parts:
			# shouldn't happen if v==0 was handled, but fallback
			return fmt_cpp_raw_int_with_error(v)
		return " | ".join(parts)

	# Individual scalars
	for name, v in sorted(scalars.items()):
		# skip style config version keys or other meta keys
		if name.lower().endswith('version'):
			continue
		# booleans
		if isinstance(v, bool):
			out.append(f"    style->{name} = {fmt_cpp_bool(v)};")
			continue
		# enums/flags
		if name in DIR_FIELDS:
			out.append(f"    style->{name} = {render_dir(v)};")
			continue
		if name in TREENODE_FLAGS_FIELDS:
			out.append(f"    style->{name} = {render_flags(v, TREENODE_SINGLETONS, TREENODE_ALIASES)};")
			continue
		if name in HOVERED_FLAGS_FIELDS:
			out.append(f"    style->{name} = {render_flags(v, HOVERED_SINGLETONS, HOVERED_ALIASES)};")
			continue
		# default: treat as float
		try:
			out.append(f"    style->{name} = {fmt_cpp_float(float(v))};")
		except Exception:
			# last resort raw text
			out.append(f"    style->{name} = {fmt_cpp_raw_int_with_error(v)};")

	out.append("")
	out.append("    ImVec4* colors = style->Colors;")
	# Colors
	for name, comps in sorted(vec4.items()):
		# require at least R,G,B,A
		r = comps.get('R', 0.0)
		g = comps.get('G', 0.0)
		b = comps.get('B', 0.0)
		a = comps.get('A', 1.0)
		enum_name = f"ImGuiCol_{name}"
		out.append(f"    colors[{enum_name}] = ImVec4({fmt_cpp_float(r)}, {fmt_cpp_float(g)}, {fmt_cpp_float(b)}, {fmt_cpp_float(a)});")

	out.append("}")

	# Print to stdout
	print('\n'.join(out))


if __name__ == '__main__':
	main()

