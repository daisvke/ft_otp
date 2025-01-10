# include "qrencode.hpp"

/*
 * QR Code Generation for TOTP Secrets
 *
 *
 * Steps to Generate the QR Code:
 *  - A TOTP URI is created dynamically using the provided secret and project name.
 *  - The URI is encoded into a QR code using the `qrencode` library.
 *  - The generated QR code is saved as a PNG file in the current directory.
 */


// Print the QR code on the terminal
void printQRCode(QRcode *qrcode, int scale = 1) {
    // Define Dimensions for the image
    int size = qrcode->width;
    int width = size * scale; // Factor to scale up each QR code module for better readability.
    int margin = 1 * scale; // Space around the QR code, scaled for better readability.
    int total_size = width + 2 * margin; // Total width size.

    /*
     * Fill and write each row
     *
     * Outer loop (y): Iterates over rows.
     * Inner loop (x): Iterates over pixels in the row.
     */

	for (int y = 0; y < total_size; ++y) {
		for (int x = 0; x < total_size; ++x) {
			if (x < margin || x >= total_size - margin ||
				y < margin || y >= total_size - margin)
			{
				std::cout << "█"; // White border
			} else {
            	// Determine if the pixel is within the QR code area.
				int module_x = (x - margin) / scale;
				int module_y = (y - margin) / scale;

				bool isModule = (module_x >= 0 && module_x < size &&
								 module_y >= 0 && module_y < size &&
								 (qrcode->data[module_y * size + module_x] & 0x01));

            	// Print the pixel as an ASCII square.
				isModule ? std::cout << " " : std::cout << "█";
			}
		}
		std::cout << std::endl;
	}
}

/*
 * The generated output file should have a size of:
 *  (QR_width×scale+2×margin)×(QR_width×scale+2×margin).
 */
void saveQRCodeAsPNG(QRcode *qrcode, const char *filename, int scale = OTP_QRCODE_SCALE) {
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
        png, info, total_size, total_size, 8,
        PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
        );
    // Write the header to the PNG file
    png_write_info(png, info);

    // Allocate row memory (png_bytep: a pointer type for PNG data)
    png_bytep row = (png_bytep)malloc(total_size);
    if (!row) {
        png_destroy_write_struct(&png, &info);
        throw PNGRowMemAllocationException();
    }

    /*
     * Fill and write each row
     *
     * Outer loop (y): Iterates over rows.
     * Inner loop (x): Iterates over pixels in the row.
     */

	for (int y = 0; y < total_size; ++y) {
		for (int x = 0; x < total_size; ++x) {
			if (x < margin || x >= total_size - margin ||
				y < margin || y >= total_size - margin)
			{
				row[x] = 0xFF; // White border
			} else {
            	// Determine if the pixel is within the QR code area.
				int module_x = (x - margin) / scale;
				int module_y = (y - margin) / scale;

				bool isModule = (module_x >= 0 && module_x < size &&
								 module_y >= 0 && module_y < size &&
								 (qrcode->data[module_y * size + module_x] & 0x01));

            	// Set pixel color (black: 0x00, white: 0xFF).
				row[x] = isModule ? 0x00 : 0xFF;
			}
		}
        // Write the row to the PNG file.
		png_write_row(png, row);
	}

    free(row);

    // Finish and clean up
    png_write_end(png, nullptr); // Finalize the PNG file
    png_destroy_write_struct(&png, &info); // Clean up libpng structures
    fclose(fp); // Close the file
}

// Create a QR code corresponding to the given URI
QRcode *generateQRCodeFromURI(const std::string secret, bool verbose)
{
    /*
    * Generate a QR Code from the given TOTP URI.
    *
    * The TOTP URI should be in Key URI Format:
    * 	otpauth://TYPE/LABEL?PARAMETERS
    *
    * - Scheme: otpauth:// specifies it's an OTP QR code.
    * - Type: totp specifies the type of OTP (time-based in this case).
    * - Label: Example:alice@example.com provides a human-readable identifier.
    *   'Example' represents the issuer = the entity (e.g., a company,
    *   application, or service) that issued the TOTP. Helps users differentiate
    *   between multiple TOTP accounts in their authenticator app.
    * - Parameters:
    * 		secret (required): The hexadecimal/base32-encoded shared secret key.
    * 		issuer (strongly recommanded): A string identifying the provider or service.
    *        It should have the same value as in 'Label'.
    *        Older versions of authenticator apps used only the label's issuer to display
    *        issuer information.
    *        Including the issuer= parameter ensures that newer apps or libraries can
    *        explicitly recognize the issuer.
    *		digits (optional): Number of digits in the OTP. Default is 6.
    *		counter: For TOTP, there is no "counter" included in the QR code because
    * 			it is derived from the current time.
    * 		period (optional): Time step in seconds. Default is 30.
    *
    * Ex.:
    * 	otpauth://totp/MyService:myuser@example.com?secret=BASE32SEED&issuer=MyService
    */

    // Create the TOTP URI from which the QR code will be generated
    std::ostringstream oss;
    oss << "otpauth://totp/"
        << OTP_PROJECT_NAME
        << ":myuser@example.com?"
        << "secret=" << secret
        << "&issuer=" << OTP_PROJECT_NAME;

    std::string totpUri = oss.str();

    if (verbose)
        std::cout << FMT_INFO " Created TOTP URI: " << totpUri << std::endl;

    // Return the QR code
    return QRcode_encodeString(totpUri.c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
}

// Create QR code from the secret key
void generateQRcodePNGFromSecret(const std::string secret, bool verbose)
{
	if (verbose)
		std::cout << FMT_INFO " Generating QR code..." << std::endl;
    try
    { 
        QRcode *qrcode = generateQRCodeFromURI(secret, verbose);

        if (qrcode) {
            std::string filetype = ".png";
            std::string filename = OTP_QRCODE_FILE + filetype;

			if (verbose)
				printQRCode(qrcode); // Print the QR code on the terminal
            saveQRCodeAsPNG(qrcode, filename.c_str()); // Save the QR code as PNG
			if (verbose)
				std::cout << FMT_DONE " Saved QR code as PNG file: '"
					<< filename << "'." << std::endl;
            QRcode_free(qrcode);
        } else {
            throw QRCodeGenerationException();
        }
    }
    catch (std::exception &e)
    {
        std::cerr << FMT_ERROR " " << e.what() << std::endl;
        throw QRCodeGenerationException();
    }
}
