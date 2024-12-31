# include "qrencode.hpp"

/*
 * The generated output file should have a size of:
 *  (QR_width×scale+2×margin)×(QR_width×scale+2×margin).
 * For a scale of 10 and margin of 4 modules,
 * it will be sufficiently large and compatible with most QR code scanners.
 */
void saveQRCodeAsPNG(QRcode *qrcode, const char *filename, int scale) {
    // Define Dimensions for the image
    int size = qrcode->width;
    int png_width = size * scale; // Factor to scale up each QR code module for better readability.
    int margin = 4 * scale; // Space around the QR code, scaled for better readability.
    int total_size = png_width + 2 * margin; // Total size of the PNG image

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
        png, info, total_size, total_size, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
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
            // Determine if the pixel is within the QR code area.
            int module_x = (x - margin) / scale;
            int module_y = (y - margin) / scale;

            // Determine if the current pixel corresponds to a black module.
            bool isModule = (module_x >= 0 && module_x < size &&
                             module_y >= 0 && module_y < size &&
                             (qrcode->data[module_y * size + module_x] & 0x01));

            // Set pixel color (black: 0x00, white: 0xFF).
            row[x] = isModule ? 0x00 : 0xFF; // Black or white
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

// Create QR code from the secret key
void generateQRcodePNGFromSecret(const std::string secret, bool verbose)
{
    try
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
    * - Parameters:
    * 		secret (required): The hexadecimal/base32-encoded shared secret key.
    * 		issuer (strongly recommanded): A string identifying the provider or service.
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
            << "&issuer=ft_otp";

        std::string totpUri = oss.str();

        if (verbose)
            std::cout << FMT_INFO " Created TOTP URI: " << totpUri << std::endl;

        QRcode *qrcode = QRcode_encodeString(totpUri.c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
        if (qrcode) {
            std::string filetype = ".png";
            std::string filename = OTP_QRCODE_FILE + filetype;

            saveQRCodeAsPNG(qrcode, filename.c_str(), 10); // Scale of 10
            QRcode_free(qrcode);
        } else {
            throw OpenFileException();
        }
    }
    catch (std::exception &e)
    {
        std::cerr << FMT_ERROR " " << e.what() << std::endl;
        exit(1);
    }
}
