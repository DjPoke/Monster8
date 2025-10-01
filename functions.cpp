#include "functions.hpp"
#include "code.hpp"

// Declare external reference to applyCode instance
extern ApplyCode applyCode;

// get pixel from pixbuf
uint32_t get_pixel(GdkPixbuf *pixbuf, int x, int y) {
    if (!pixbuf) return 0;

    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    if (x < 0 || x >= width || y < 0 || y >= height)
        return 0; // hors limites

    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar* pixels = gdk_pixbuf_get_pixels(pixbuf);

    guchar* p = pixels + y * rowstride + x * n_channels;

    guchar red   = p[0];
    guchar green = p[1];
    guchar blue  = p[2];
    // guchar alpha = (n_channels == 4) ? p[3] : 255; // unused for now

    return (red << 16) | (green << 8) | blue;
}

// copy pixbuf image into memory at addr
uint32_t CopyImageToRaw(GdkPixbuf *pixbuf, uint32_t addr, uint32_t xframe, uint32_t yframe, uint32_t width, uint32_t height) {
    if (width <= 0 || height <= 0) {
        return addr;
    }

    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            // get RGB pixel from pixbuf
            uint32_t c = get_pixel(pixbuf, x + xframe, y + yframe);

            uint8_t r = ((c >> 16) & 0xFF) >> 5; // 3 bits
            uint8_t g = ((c >> 8) & 0xFF) >> 6; // 2 bits
            uint8_t b = (c & 0xFF) >> 5; // 3 bits

            // 8 bits color
            uint8_t c8 = ((r << 5) | (g << 3) | b);

            applyCode.memory[addr] = c8;
            
            addr++;
        }
    }

    return addr;
}

GdkPixbuf* ConvertImageTo24bits(uint32_t address, uint32_t width, uint32_t height) {
    // Créer une image Pixbuf vide de la taille width, height
    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                       TRUE,  // has_alpha
                                       8,     // bits_per_sample
                                       width,
                                       height);
    
    if (pixbuf != NULL) {
        // Initialiser avec une couleur transparente
        gdk_pixbuf_fill(pixbuf, 0x00000000); // RGBA: transparent

        for(uint32_t y = 0; y < height; y++) {
            for(uint32_t x = 0; x < width; x++) {
                // Calculer l'adresse de chaque pixel dans la mémoire
                uint32_t mem_addr = address + (y * width + x);

                // calculer la couleur RGB par rapport à la palette
                uint8_t c = applyCode.memory[mem_addr];

                uint8_t red = (c & 0xC0);
                uint8_t green = (c & 0x18) << 3;
                uint8_t blue = (c & 0x07) << 5;

                // Écrire un pixel dans le Pixbuf aux coordonnées x, y
                guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
                int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
                int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
                
                guchar *pixel = pixels + y * rowstride + x * n_channels;
                pixel[0] = red;   // Rouge
                pixel[1] = green; // Vert
                pixel[2] = blue;  // Bleu
                pixel[3] = 255;   // Alpha (opaque)

            }
        }
        
        // Retourner le Pixbuf créé
        return pixbuf;
    }
    
    // Retourner NULL si échec
    return NULL;
}

// function to explode
std::vector<std::string> Explode(const std::string &input, const std::string &delim) {
    std::vector<std::string> res;
    if (delim.empty()) {
        res.push_back(input);
        return res;
    }

    std::size_t start = 0;
    while (start <= input.length()) {
        std::size_t pos = input.find(delim, start);
        if (pos == std::string::npos) {
            std::string token = input.substr(start);
            res.push_back(token.empty() ? " " : token);
            break;
        }
        std::string token = input.substr(start, pos - start);
        res.push_back(token.empty() ? " " : token);
        start = pos + delim.length();
    }
    return res;
}

// StringField : retourne le n-ième token ou chaîne vide si hors limites
std::string StringField(const std::string &str, int n, const std::string &delim) {
    const auto tokens = Explode(str, delim);
    if (n <= 0 || static_cast<size_t>(n) > tokens.size()) {
        return ""; // hors limites → renvoie une chaîne vide
    }
    return tokens[n - 1];
}

// Conversion string → int sécurisée
bool StringToInt(const std::string &str, int &outValue) {
    try {
        size_t idx;
        outValue = std::stoi(str, &idx);
        if (idx != str.length()) {
            // il reste des caractères non numériques
            return false;
        }
        return true;
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

// CountString : nombre d’occurrences du séparateur
uint32_t CountString(const std::string &str, const std::string &delim) {
    if (delim.empty()) {
        std::cout << "Error: Empty separator" << std::endl;
        return 0;
    }

    const auto tokens = Explode(str, delim);
    if (tokens.empty()) {
        std::cout << "Error: Empty string" << std::endl;
        return 0;
    }
    
    return (uint32_t)tokens.size();
}

// trim from the start (in place)
void LTrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from the end (in place)
void RTrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
void Trim(std::string &s) {
    RTrim(s);
    LTrim(s);
}

// convert string to integer
int Val(std::string s) {
    Trim(s);

    if(!s.empty()) {
        const bool hasLowerPrefix = s.rfind("0x", 0) == 0;
        const bool hasUpperPrefix = s.rfind("0X", 0) == 0;

        if(hasLowerPrefix || hasUpperPrefix) {
            return static_cast<int>(std::stoul(s, nullptr, 16));
        } else {
            int outValue;

            StringToInt(s, outValue);

            return outValue;
        }
    } else {
        std::cout << "Error: Empty string" << std::endl;
        exit(EXIT_FAILURE);
    }
}
