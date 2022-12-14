//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include <vector>
#include "Scene.hpp"
#include "Renderer.hpp"

typedef unsigned char uchar;

inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

void dkernel(double*& f, int size, double sigma)
{
	double sum = 0;
	int len = size / 2;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			f[i * size + j] = exp(-(((i - len) * (i - len)) + ((j - len) * (j - len))) / (2.0 * sigma * sigma));
		}
	}
}

void BFilter(const std::vector<uchar>& f, std::vector<uchar>& g, double*& d,
			size_t width, size_t height, double sigma, size_t size)
{
	int len = size / 2;
	g = f;
	for (int k = 0; k < height; k++) {
		for (int l = 0; l < width; l++) {
			double sum_1 = 0;
			double sum_2 = 0;
			double tmp = 0;
			int window = 0;
			for (int i = k - len; i < k + len + 1; i++) {
				for (int j = l - 3 * len; j < l + 3 * len + 1; j += 3) {
					if (i < 0 || j < 0 || i > height - 1 || j > width - 1) continue;
					tmp = d[window] * exp(-(f[i * width + j] - f[k * width + l]) * (f[i * width + j] - f[k * width + l]) / (2.0 * sigma * sigma)); 
					sum_1 += f[i * width + j] * tmp;
					sum_2 += tmp;
					window++;
				}
			}
			sum_1 /= sum_2;
			if (sum_1 > 255)
				sum_1 = 255;
			if (sum_1 < 0)
				sum_1 = 0;
			g[k * width + l] = sum_1;
		}
	}
}

void MFilter(const std::vector<uchar>& f, std::vector<uchar>& g, size_t width, size_t height, size_t size) {
    int len = size / 2;
    for (int k = 0; k < height; k++) {
        for (int l = 0; l < width; l++) {
            static std::vector<uchar> v;
            int window = 0;
            for (int i = k - len; i < k + len + 1; i++) {
                for (int j = l - 3 * len; j < l + 3 * len + 1; j+= 3) {
                    if (i < 0 || j < 0 || i > height - 1 || j > width - 1) continue;
                    v.push_back(f[i * width + j]);
                }
            }
            std::sort(v.begin(), v.end());
            g[k * width + l] = v[v.size() / 2];
            v.clear();
        }
    }
}


const float EPSILON = 0.00001;

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);
    int m = 0;

    // change the spp value to change sample ammount
    int spp = 16;
    std::cout << "SPP: " << spp << "\n";
    for (uint32_t j = 0; j < scene.height; ++j) {
        for (uint32_t i = 0; i < scene.width; ++i) {
            // generate primary ray direction
            float x = (2 * (i + 0.5) / (float)scene.width - 1) *
                      imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

            Vector3f dir = normalize(Vector3f(-x, y, 1));
            for (int k = 0; k < spp; k++){
                framebuffer[m] += scene.castRay(Ray(eye_pos, dir), 0) / spp;  
            }
            m++;
        }
        UpdateProgress(j / (float)scene.height);
    }
    UpdateProgress(1.f);

    std::vector<uchar> img(scene.width * scene.height * 3);
    for (int i = 0; i < scene.height * scene.width; ++i) {
        img[3*i] = (uchar)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
        img[3*i + 1] = (uchar)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
        img[3*i + 2] = (uchar)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
    }
    double sigma_1 = 10;
	double sigma_2 = 50;
	int window = 5;	
    double* k;
	k = new double[window * window];
	dkernel(k, window, sigma_1);
    std::vector<uchar> final_img(scene.width * scene.height * 3);
	//BFilter(img, final_img, k, int(scene.width) * 3, scene.height, sigma_2, window);
    //MFilter(img, final_img, int(scene.width) * 3, scene.height, window); 
    final_img = img;

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = final_img[i*3];
        color[1] = final_img[i*3 + 1];
        color[2] = final_img[i*3 + 2];
        fwrite(color, 1, 3, fp);
    }
    //for (auto i = 0; i < scene.height * scene.width; ++i) {
    //    static unsigned char color[3];
    //    color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
    //    color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
    //    color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
    //    fwrite(color, 1, 3, fp);
    //}
    fclose(fp);    
    delete []k;
}
