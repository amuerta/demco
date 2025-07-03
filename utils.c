
#ifndef __DEMCO_UTILITIES_H
#define __DEMCO_UTILITIES_H

#include "defs.h"

Rectangle RectangleExpand(Rectangle r,float s) {
	return REC {
		r.x - s,
		r.y - s,
		r.w + s*2,
		r.h + s*2,
	};
}

Rectangle Ui_hchop(Rectangle src, float s) {
	return REC {
		UnfoldVec2(src),
		src.w-s,src.h
	};
}

Rectangle Ui_vchop(Rectangle src, float s) {
	return REC {
		UnfoldVec2(src),
		src.w,src.h-s
	};
}

Rectangle Ui_hstep(Rectangle src, float s) {
	return REC {
		src.x + s,src.y,
		RecDimensions(src)
	};
}

Rectangle Ui_vstep(Rectangle src, float s) {
	return REC {
		src.x,src.y+s,
		RecDimensions(src)
	};
}

void Ui_hswap(Rectangle* src) {
	// left / right
	// Original < left / right >
	Rectangle l, r;
	Rectangle ol = src[0],
			  or = src[1];

	l = REC {
		ol.x, ol.y,
		or.w, or.h
	};
	r = REC {
		ol.x + or.w, ol.y ,
		ol.w, ol.h
	};

	src[0] = l;
	src[1] = r;
}

void Ui_vswap(Rectangle* src) {
	// left / right
	// Original < left / right >
	Rectangle l, r;
	Rectangle ol = src[0],
			  or = src[1];

	l = REC {
		ol.x, ol.y,
		or.w, or.h
	};
	r = REC {
		ol.x, ol.y + or.h,
		ol.w, ol.h
	};

	src[0] = l;
	src[1] = r;
}

float Ui_percent(float value, float perc) {
	return value * perc / 100;
}

void Ui_vsplitn(Rectangle *result, Rectangle source, float n) {
	for(u32 i = 0; i < n; i++)
		result[i] = REC {
			source.x,
			source.y + (source.h/n)*i,
			source.w,
			(source.h/n)
		};
}

void Ui_vsplit(Rectangle *result, Rectangle source, float size) {
	result[0] = REC {
		source.x, source.y,
		source.w, size
	};
	result[1] = REC {
		source.x, source.y + size,
		source.w, source.h - size
	};
}

void Ui_hsplit(Rectangle *result, Rectangle source, float size) {
	result[0] = REC {
		source.x, source.y,
			size, source.h,
	};
	result[1] = REC {
		source.x + size, source.y,
		source.w - size, source.h,
	};
}

void GuiOutlineRec(Rectangle rec, const char* text) {
	GuiDummyRec(rec,text);
	DrawRectangleLinesEx(rec,1,DARKGRAY);
}

#endif // __DEMCO_UTILITIES_H
