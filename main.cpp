#include <vector>

#include <octave/octave.h>
#include <octave/parse.h>
#include <octave/builtin-defun-decls.h>

extern "C" {
#include "iio.h"
}

struct Image {
    float* pixels;
    int w, h, d;

    Image(float* pixels, int w, int h, int d)
        : pixels(pixels), w(w), h(h), d(d) {}
};

static Image run_octave(const std::string& prog, std::vector<Image> images)
{
    static octave::interpreter* app;

    if (!app) {
        app = new octave::interpreter();
        app->initialize_history(false);
        app->initialize();
        app->interactive(false);
    }

    try {
        octave_value_list in;

        // create the function
        octave_value_list in2;
        in2(0) = octave_value(prog);
        octave_value_list fs = Fstr2func(in2);
        octave_function* f = fs(0).function_value();

        // create the matrices
        for (int i = 0; i < images.size(); i++) {
            const Image img = images[i];
            dim_vector size(img.h, img.w, img.d);
            NDArray m(size);

            float* xptr = img.pixels;
            for (int y = 0; y < img.h; y++) {
                for (int x = 0; x < img.w; x++) {
                    for (int z = 0; z < img.d; z++) {
                        m(y, x, z) = *(xptr++);
                    }
                }
            }

            in(i) = octave_value(m);
        }

        // eval
        octave_value_list out = octave::feval(f, in, 1);

        if (out.length() > 0) {
            NDArray m = out(0).array_value();
            int w = m.cols();
            int h = m.rows();
            int d = m.ndims() == 3 ? m.pages() : 1;
            size_t size = w * h * d;
            if (size == 1) {
                printf("%f\n", m(0,0,0));
                return Image(0,0,0,0);
            }
            float* data = (float*) malloc(sizeof(float) * size);
            float* ptrdata = data;
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    for (int z = 0; z < d; z++) {
                        *(ptrdata++) = m(y, x, z);
                    }
                }
            }
            return Image(data, w, h, d);
        } else {
            std::cerr << "no image returned from octave\n";
        }

    } catch (const octave::exit_exception& ex) {
        exit (ex.exit_status());

    } catch (const octave::execution_exception&) {
        std::cerr << "error evaluating Octave code!" << std::endl;
    }
    return Image(0,0,0,0);
}

static char *pick_option(int *c, char ***v, char *o, char *d)
{
    int argc = *c;
    char **argv = *v;
    for (int i = 0; i < argc - 1; i++)
        if (argv[i][0] == '-' && 0 == strcmp(argv[i]+1, o))
        {
            char *r = argv[i+1];
            *c -= 2;
            for (int j = i; j < argc - 1; j++)
                (*v)[j] = (*v)[j+2];
            return r;
        }
    return d;
}

static int main_images(int c, char **v)
{
    if (c < 2) {
        fprintf(stderr, "usage:\n\t%s in1 in2 ... \"octave expression\" [-o output]\n", *v);
        return EXIT_FAILURE;
    }

    char *filename_out = pick_option(&c, &v, "o", "-");

    int n = c - 2;

    std::vector<Image> images;
    for (int i = 0; i < n; i++) {
        int w, h, d;
        float* p = iio_read_image_float_vec(v[i+1], &w, &h, &d);
        images.push_back(Image(p, w, h, d));
    }

    std::string prog = "@(";
    for (int i = 0; i < n; i++) {
        if (i < 3)
            prog += 'x'+i;
        else
            prog += 'a'+i;
        if (i != n-1)
            prog += ',';
    }
    prog += ") ";
    prog += v[c-1];

    Image res = run_octave(prog, images);

    if (res.pixels) {
        iio_write_image_float_vec(filename_out, res.pixels, res.w, res.h, res.d);
        free(res.pixels);
    }

    for (int i = 0; i < n; i++) {
        free(images[i].pixels);
    }
    return 0;
}

int main(int c, char** v)
{
    if (c == 2) {
        char *vv[3] = { v[0], "-", v[1] };
        return main_images(3, vv);
    }

    return main_images(c,v);
}

