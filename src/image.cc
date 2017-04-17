#include "image.hh"

bool Image::Send_Rect(Canvas &canvas, 
		      GdkRectangle dst,
		      GdkPoint src)
{
	assert (Is());

	gdk_draw_image(canvas.Get_Drawable(),
		       canvas.Get_GC(),
		       image,
		       src.x, src.y,
		       dst.x, dst.y,
		       dst.width, dst.height);

	return true; 
}

bool Image::Receive_Rect(const Canvas &canvas,
			 GdkRectangle rect)
{
	assert (! Is());

	image= gdk_image_get(canvas.Get_Drawable(),
			     rect.x,
			     rect.y,
			     rect.width,
			     rect.height);

	return image != NULL; 
}

bool Image::Draw_Stretch_Canvas(Canvas &dst,
				const Canvas &src,
				GdkRectangle &dst_rect,
				GdkRectangle &src_rect)
{
	Image src_image;

	if (! src_image.Receive_Rect(src, src_rect))
		return false;

	Image dst_image;

	if (! dst_image.Create((GdkPoint){dst_rect.width, dst_rect.height}))
		return false;

  
	for (int x= 0; x < dst_rect.width; ++ x) 
	{
		for (int y= 0; y < dst_rect.height; ++ y)
		{
			int r= 0, g= 0, b= 0;
//			if (anti_alias)
//			{
				for (int xx= src_rect.width * x / dst_rect.width;
				     xx < src_rect.width * (x + 1) / dst_rect.width;
				     ++ xx)
					for (int yy= src_rect.height * y / dst_rect.height;
					     yy < src_rect.height * (y + 1) / dst_rect.height;
					     ++ yy)
					{
						RGB rgb;
						src_image.Get_Pixel(xx, yy,
								    rgb.r, rgb.g, rgb.b);
						r += rgb.r;
						g += rgb.g;
						b += rgb.b;
					}
				int count= ((src_rect.width * (x + 1) / dst_rect.width
					     - src_rect.width * x / dst_rect.width)
					    * (src_rect.height * (y + 1) / dst_rect.height
					       - src_rect.height * y / dst_rect.height));
				r /= count;
				g /= count;
				b /= count;
//			}
//			else
//			{
//				src_image.Get_Pixel(src_rect.width * x / dst_rect.width,
//						    src_rect.height * y / dst_rect.height,
//						    r, g, b);
//			}

			dst_image.Draw_Pixel(x, y, r, g, b); 
		}
	}


	if (! dst_image.Send_Rect(dst, dst_rect, (GdkPoint){0, 0}))
		return false;

	return true; 
}

void Image:: Draw_Pixel(int x,
			int y,
			int r,
			int g,
			int b)
{
	assert (Is());

	GdkColor color;
	color.red= r << 8;
	color.green= g << 8;
	color.blue= b << 8; 
	gdk_colormap_alloc_color(gdk_colormap_get_system(),
				 &color,
				 FALSE,
				 TRUE);
	gdk_image_put_pixel(image, x, y, color.pixel); 
			
}

void Image::Free()
{
	if (! Is())
		return;
	gdk_image_destroy(image);
	image= NULL; 
}

bool Image::Create(GdkPoint size)
{
	assert (size.x > 0 && size.y > 0); 
	assert (! Is()); 

	image= gdk_image_new(GDK_IMAGE_FASTEST,
			     gdk_rgb_get_visual(),
			     size.x,
			     size.y);

	return image != NULL; 
}
