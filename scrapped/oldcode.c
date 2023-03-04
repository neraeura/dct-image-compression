// A2Methods_UArray2
// RGBtoFloat(A2Methods_UArray2 processed_image, A2Methods_T methods, Mapfun map,
//                                                          unsigned denominator)
// {
//         int width = methods->width(processed_image);
//         int height = methods->height(processed_image);

//         /* create a new UArray2 */
//         A2Methods_UArray2 converted_image = methods->new(width, height,
//                                                         sizeof(struct Pnm_rgb_flt));
//         assert(converted_image != NULL);

//         for (int col = 0; col < width; col++) {
//                 for (int row = 0; row < height; row++) {
//                         A2Methods_Object *pixel = methods->at(processed_image, col, row);

//                         /* convert from scaled to float */
//                         float r = (float) ((Pnm_rgb)pixel)->red;
//                         float g = (float) ((Pnm_rgb)pixel)->green;
//                         float b = (float) ((Pnm_rgb)pixel)->blue;
                        
//                         /* TODO: fix denominator issue?? */
//                         float flt_r = r /(float) denominator;
//                         float flt_g = g /(float) denominator; 
//                         float flt_b = b /(float) denominator;

//                         /* Get the index of an element in this new array */
//                         Pnm_rgb_flt new_index = (Pnm_rgb_flt) methods->at(converted_image, col, row);
//                         /* Create a new_pixel of float representation */
//                         Pnm_rgb_flt new_pixel = create_flt_pixel(flt_r, flt_g, flt_b);
//                         /* Put that pixel in converted_image @ new_index */
//                         *new_index = *new_pixel;
//                 }
//         }
//         /* Frees processed_image */
//         methods->free(&processed_image);
//         return converted_image;
// }


// A2Methods_UArray2
// RGBtoInt(A2Methods_UArray2 RGB_image, Mapfun map, A2Methods_T methods)
// {

//         (void) map;

//         int width = methods->width(RGB_image);
//         int height = methods->height(RGB_image);
//         int denominator = 255; 


//         A2Methods_UArray2 converted_image = methods->new(width, height,
//                                                         sizeof(struct Pnm_rgb));

//         for (int col = 0; col < width; col++) {
//                 for (int row = 0; row < height; row++) {
//                         A2Methods_Object *pixel = methods->at(RGB_image, col, row);

//                         /* convert from scaled to float */
//                         int r = (int) ((Pnm_rgb_flt) pixel)->red * denominator;
//                         int g = (int) ((Pnm_rgb_flt) pixel)->green * denominator;
//                         int b = (int) ((Pnm_rgb_flt) pixel)->blue * denominator;


//                         Pnm_rgb newpixel = methods->at(converted_image, col, row);
//                         *newpixel = *((Pnm_rgb) create_int_pixel(r, g, b));
//                 }
//         }
//         /* Frees RGB_image */
//         methods->free(&RGB_image);
//         return converted_image;
// }


/* Convert from Component Video Color Space to RGB */
// A2Methods_UArray2 
// ComponentVideotoRGB(A2Methods_UArray2 component_image, Mapfun map, A2Methods_T methods)
// {
//         (void) map;
        
//         int width = methods->width(component_image);
//         int height = methods->height(component_image);

//         A2Methods_UArray2 converted_image = methods->new(width, height,
//                                                         sizeof(struct Pnm_rgb_flt));

//         for (int col = 0; col < width; col++) {
//                 for (int row = 0; row < height; row++) {
//                         Pnm_componentvid_flt pixel = (Pnm_componentvid_flt) methods->at(component_image, col, row);
//                         float pr = (float) ((Pnm_componentvid_flt) pixel)->pr;
//                         float y = (float) ((Pnm_componentvid_flt) pixel)->y;
//                         float pb = (float) ((Pnm_componentvid_flt) pixel)->pb;

//                         float r = 1.0 * y + 0.0 + pb + 1.402 * pr; 
//                         float g = 1.0 * y - 0.344136 * pb - 0.714136 * pr; 
//                         float b = 1.0 * y + 1.772 * pb + 0.0 * pr;

//                         Pnm_rgb_flt new_index = methods->at(converted_image, col, row);
//                         Pnm_rgb_flt new_pixel = create_flt_pixel(r, g, b);
//                         *new_index = *new_pixel;

//                 }
//         }
//         /* Frees component_image */
//         methods->free(&component_image);
//         return converted_image;
// }



// A2Methods_UArray2
// RGBtoComponentVideo(A2Methods_UArray2 fltRGB_image, A2Methods_T methods, 
//                                                                 Mapfun map)
// {
//         (void) map;
//         int width = methods->width(fltRGB_image);
//         int height = methods->height(fltRGB_image);

//         A2Methods_UArray2 converted_image = methods->new(width, height,
//                                                 sizeof(struct Pnm_componentvid_flt));

//         for (int col = 0; col < width; col++) {
//                 for (int row = 0; row < height; row++) {
//                         Pnm_rgb_flt pixel = (Pnm_rgb_flt) methods->at(fltRGB_image, col, row);
                        
//                         float r = (float) ((Pnm_rgb_flt) pixel)->red;
//                         float g = (float) ((Pnm_rgb_flt) pixel)->green;
//                         float b = (float) ((Pnm_rgb_flt) pixel)->blue;

//                         float y = 0.299 * r + 0.587 * g + 0.114 * b;
//                         float pb = -0.168736 * r - 0.331264 * g + 0.114 * b;
//                         float pr = 0.5 * r - 0.418688 * g - 0.081312 * b;


//                         Pnm_componentvid_flt new_index = (Pnm_componentvid_flt) methods->at(converted_image, col, row);
//                         Pnm_componentvid_flt new_pixel = create_componentvid_pixel(y, pr, pb);
//                         *new_index = *new_pixel;
//                 }
//         }

//         /* Frees fltRGB_image */ 
//         methods->free(&fltRGB_image);
//         return converted_image;
// }


// A2Methods_UArray2
// RGBtoInt(A2Methods_UArray2 RGB_image, Mapfun map, A2Methods_T methods)
// {

//         (void) map;

//         int width = methods->width(RGB_image);
//         int height = methods->height(RGB_image);
//         int denominator = 255; 


//         A2Methods_UArray2 converted_image = methods->new(width, height,
//                                                         sizeof(struct Pnm_rgb));

//         for (int col = 0; col < width; col++) {
//                 for (int row = 0; row < height; row++) {
//                         A2Methods_Object *pixel = methods->at(RGB_image, col, row);

//                         /* convert from scaled to float */
//                         int r = (int) ((Pnm_rgb_flt) pixel)->red * denominator;
//                         int g = (int) ((Pnm_rgb_flt) pixel)->green * denominator;
//                         int b = (int) ((Pnm_rgb_flt) pixel)->blue * denominator;


//                         Pnm_rgb newpixel = methods->at(converted_image, col, row);
//                         *newpixel = *((Pnm_rgb) create_int_pixel(r, g, b));
//                 }
//         }
//         /* Frees RGB_image */
//         methods->free(&RGB_image);
//         return converted_image;
// }
