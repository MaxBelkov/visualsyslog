//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "gr.h"

//---------------------------------------------------------------------------
static double hue2rgb(double p, double q, double t){
	if(t < 0) t += 1;
	if(t > 1) t -= 1;
	if(t < 1.0/6) return p + (q - p) * 6 * t;
	if(t < 1.0/2) return q;
	if(t < 2.0/3) return p + (q - p) * (2.0/3 - t) * 6;
	return p;
}
//---------------------------------------------------------------------------
TColor fromHSL(double h,double s,double l){
    double r, g, b;

    if(s == 0){
        r = g = b = l;
    } else{
        double q = l < 0.5 ? l * (s + 1) : l + s - l * s;
        double p = 2 * l - q;
        r = hue2rgb(p, q, h + 1.0/3);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0/3);
	}

	int rr=r*255+0.5;
	int gg=g*255+0.5;
	int bb=b*255+0.5;

	return (TColor)  RGB(rr, gg, bb);
}
//---------------------------------------------------------------------------
void toHSL(TColor color,double *hh,double *ss,double *ll){
try // catch division by zero
{
	double r=1.0*GetRValue(color)/255;
	double g=1.0*GetGValue(color)/255;
	double b=1.0*GetBValue(color)/255;

	double max=r; if(g>max) max=g; if(b>max) max=b;
	double min=r; if(g<min) min=g; if(b<min) min=b;

	double h, s, l = (max + min) / 2;

    if(max == min){
        h = s = 0; // achromatic
    }else{
        double d = max - min;
        s = l > 0.5 ? d / (2 - max - min) : d / (max + min);

		if(max==r) h = (g - b) / d + (g < b ? 6 : 0);
		else if(max==g) h = (b - r) / d + 2;
		else h = (r - g) / d + 4;

		h /= 6;
	}

	if(hh) *hh=h;
	if(ss) *ss=s;
	if(ll) *ll=l;
}
catch(...)
{}
}
//---------------------------------------------------------------------------
void __fastcall FillVolume(TCanvas * c, TRect rect, TColor color,
  double brightness_range_koeff)
{
  int w = rect.right - rect.left;
  int h = rect.bottom - rect.top;

  if( ! c || h == 0 )
    return;

  double hh, ss, ll;
  // ColorToRGB: clBtnFace const to color value
  toHSL((TColor)ColorToRGB(color), &hh, &ss, &ll);

  double brightness_range = (ll > 0.5) ? (1.0 - ll) : ll;
  brightness_range *= brightness_range_koeff;

  double brightness_step = brightness_range * 2 / h;

  // save
  TBrushStyle bs = c->Brush->Style;
  TColor bc = c->Brush->Color;

  c->Brush->Style = bsSolid;

  for(int i=0; i<h; i++)
  {
    c->Brush->Color = fromHSL(hh, ss, ll + brightness_range - brightness_step * i);
    c->FillRect(Rect(rect.Left, rect.Top + i, rect.Left + w, rect.Top + i + 1));
  }

  // restore
  c->Brush->Color = bc;
  c->Brush->Style = bs;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
