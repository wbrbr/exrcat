#include <iostream>
#include <vector>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImathBox.h>
#include <assert.h>
#include <memory>

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input files>... <output>" << std::endl;
        exit(1);
    }

    std::vector<std::string> input_paths;
    for (unsigned int i = 1; i < argc-1; i++) {
        input_paths.push_back(std::string(argv[i]));
    }

    // have to use a unique_ptr because RgbaInputFile is non-movable
    std::vector<std::unique_ptr<Imf::RgbaInputFile>> input_files;
    std::vector<Imath::V2i> input_sizes;

    for (const std::string& path : input_paths) {
        std::unique_ptr<Imf::RgbaInputFile> f = std::make_unique<Imf::RgbaInputFile>(path.c_str());
        Imath::Box2i box = f->dataWindow();
        assert(box.min == Imath::V2i(0, 0));
        int width = box.max.x+1;
        int height = box.max.y+1;

        input_sizes.push_back({width, height});
        input_files.push_back(std::move(f));
    }

    assert(input_files.size() == input_paths.size());

    Imath::V2i final_size(0, 0);
    for (Imath::V2i size : input_sizes) {
        final_size.x += size.x;
        final_size.y = max(final_size.y, size.y);
    }

    std::vector<Imf::Rgba> buf;
    buf.resize(final_size.x * final_size.y);

    unsigned int x_offset = 0;
    for (unsigned int i = 0; i < input_files.size(); i++) {
        input_files[i]->setFrameBuffer(&buf[final_size.y*x_offset], final_size.y, 1);
        input_files[i]->readPixels(0, input_sizes[i].y-1);
        x_offset += input_sizes[i].x;
    }

    Imf::RgbaOutputFile file(argv[argc-1], final_size.x, final_size.y, Imf::WRITE_RGBA);
    file.setFrameBuffer(buf.data(), final_size.y, 1);
    file.writePixels(final_size.y);
    return 0;
}
