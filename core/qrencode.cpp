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
				printQRCode(qrcode, 1); // Print the QR code on the terminal
            saveQRCodeAsPNG(qrcode, filename.c_str(), OTP_QRCODE_SCALE); // Save the QR code as PNG
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
