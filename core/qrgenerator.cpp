#include "qrgenerator.hpp"

/*
 * qrgenerator
 * -----------
 * 
 * This program creates a QR code with the string given from the
 * command line, then:
 *  - It prints the QR code on the terminal.
 *  - It saves the QR code as a PNG file.
 *
 * You can verify the generated code with:
 *  sudo apt install zbar-tools
 *  zbarimg qrcode.png
 */


/*
 * Process each row of the QR code.
 *
 * In PNG saving mode:
 *  Fill and write each row
 * 
 * In terminal print mode:
 *  Print each row on the terminal
 *
 */
void processQRCodeRows(
    QRcode *qrcode, int size, int total_size, int margin, int scale,
    png_structp *png = nullptr, png_bytep row = nullptr
    )
{
    /*
     * Outer loop (y): Iterates over rows.
     * Inner loop (x): Iterates over units in the row.
     */
	for (int y = 0; y < total_size; ++y) {
		for (int x = 0; x < total_size; ++x) {
			// Check if the unit is a margin
			if (x < margin || x >= total_size - margin ||
				y < margin || y >= total_size - margin)
			{
                if (row) row[x] = 0xFF; // For PNG saving mode 
                else std::cout << "██"; // For terminal printing mode
			} else {
				/* If the unit is a module (black or white square dot):
				 *
				 * To check whether the current module is black or white,
				 * we need to get the corresponding module in the original
				 * unscaled qrcode.
				 * To get those coordinates we need to remove the applied 
				 * margin and scale.
				 */
				int module_x = (x - margin) / scale;
				int module_y = (y - margin) / scale;

				bool isBlackModule = (
									// Check if the coordinates are within the QR code width
									module_x >= 0 && module_x < size &&
								 	module_y >= 0 && module_y < size &&
									/* 
									 * The QR code is stored as an unsigned char array.
                                     * Thus, in order to get the module from the coordinates we do:
                                     *  module = qrcode[y * size + x]
                                     * 
                                     * Furthermore, a value of module = 0x00 corresponds to a white dot
                                     * and 0x01 to a black one.
									 */
								 	(qrcode->data[module_y * size + module_x] & 0x01));

            	// Print/save the module as a black or white square.
				if (isBlackModule) {
                    if (row) row[x] = 0x00;
                    else std::cout << "  ";
                } else {
                    if (row) row[x] = 0xFF;
                    else std::cout << "██";
                }
			}
		}
        // Write the row to the PNG file.
        if (row) png_write_row(*png, row);
		else std::cout << std::endl;
	}
    if (row) free(row);
}

// Print the QR code on the terminal
void printQRCode(QRcode* &qrcode, const int scale) {
    // Define Dimensions for the image
    int size = qrcode->width;
    int width = size * scale; // Factor to scale up each QR code module for better readability.
    int margin = 1 * scale; // Space around the QR code, scaled for better readability.
    int total_size = width + 2 * margin; // Total width size.

    // Print each row on the terminal
	processQRCodeRows(qrcode, size, total_size, margin, scale);
}

png_infop	init_png(FILE* &fp, png_structp &png, int size)
{
    // Initialize the PNG info structure.
    png_infop info = png_create_info_struct(png);
    if (!info) {
        // Clean up if info structure creation fails.
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        throw LibpngInitException();
    }

    /* 
     * setjmp: Sets a point for error recovery.
     *  If an error occurs during PNG creation, execution jumps here.
     *
     * png_jmpbuf: Retrieves the jump buffer for the PNG structure.
     */
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        throw PNGCreationException();
    }

    png_init_io(png, fp);

    // Write PNG header
    png_set_IHDR(
        png, info, size, size, 8,
        PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
        );
    // Write the header to the PNG file
    png_write_info(png, info);

	return info;
}

/*
 * The generated output file should have a size of:
 *  (QR_width×scale+2×margin)×(QR_width×scale+2×margin).
 */
void saveQRCodeAsPNG(
	QRcode* &qrcode, const char* filename, const int scale) {
    // Define Dimensions for the image
    int size = qrcode->width;
    int png_width = size * scale; // Factor to scale up each QR code module for better readability.
    int margin = 4 * scale; // Space around the QR code, scaled for better readability.
    int total_size = png_width + 2 * margin; // Total width size of the PNG image (square)

    // Open the outfile in binary write mode.
    FILE *fp = fopen(filename, "wb");
    if (!fp)
        throw OpenFileException();

    /*
     * Initialize libpng
     *
     * png_create_write_struct: Initializes the PNG write structure.
     * PNG_LIBPNG_VER_STRING: Uses the version of libpng linked at compile time.
     */
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        fclose(fp);
        throw LibpngInitException();
    }

	png_infop info = init_png(fp, png, total_size);

    // Allocate row memory (png_bytep: a pointer type for PNG data)
    png_bytep row = (png_bytep)malloc(total_size);
    if (!row) {
        png_destroy_write_struct(&png, &info);
        throw PNGRowMemAllocationException();
    }

    // Fill and write each row on the PNG structure
    processQRCodeRows(qrcode, size, total_size, margin, scale, &png, row);

    // Finish and clean up
    png_write_end(png, nullptr); // Finalize the PNG file
    png_destroy_write_struct(&png, &info); // Clean up libpng structures
    fclose(fp); // Close the file
}
