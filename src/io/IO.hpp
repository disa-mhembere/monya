#ifndef MONYA_I0_HPP__
#define MONYA_I0_HPP__

#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <utility>

#include "../common/exception.hpp"
#include "../common/types.hpp"
#include "../utils/FileUtil.hpp"

namespace monya { namespace io {

template <typename T>
    void print_arr(const T* arr, const unsigned len) {
#ifndef BIND
        printf("[ ");
        for (unsigned i = 0; i < len; i++) {
            std::cout << arr[i] << " ";
        }
        printf("]\n");
#endif
    }

template <typename T>
void print_mat(T* matrix, const unsigned rows, const unsigned cols) {
#ifndef BIND
    for (unsigned row = 0; row < rows; row++) {
        std::cout << "[";
        for (unsigned col = 0; col < cols; col++) {
            std::cout << " " << matrix[row*cols + col];
        }
        std::cout <<  " ]\n";
    }
#endif
}

class IO {
    protected:
        std::string fn;
        size_t dtype_size;
        data_t* data;
        MAT_ORIENT orientation;
        dimpair dim;

    public:
        typedef IO* raw_ptr;

        IO() {
            data = NULL;
            fn = "";
            orientation = MAT_ORIENT::INVALID;
            dtype_size = sizeof(data_t);
        }

        virtual void transpose() {
            throw not_implemented_exception(__FILE__, __LINE__);
        }

        virtual void print() {
            printf("fn: %s\n dypte size: %lu, orientation: %s, shape:"
                " (%lu, %lu)\n", fn.c_str(), dtype_size,
                (orientation == MAT_ORIENT::COL ? "COL" : "ROW"),
                 dim.first, dim.second);
        }

        IO(const std::string fn): IO() {
            this->fn = fn;
        }

        IO(const dimpair dim, MAT_ORIENT orient): IO() {
            this->dim = dim;
            this->orientation = orient;
        }

        void set_orientation(const MAT_ORIENT orient) {
            this->orientation = orient;
        }

        const MAT_ORIENT get_orientation() const {
            return this->orientation;
        }

        virtual void set_fn(const std::string fn) {
            this->fn = fn;
        }

        const std::string get_fn() const {
            return this->fn;
        }

        // Read everything in the file
        virtual void read(data_t* buf) {
            throw not_implemented_exception(__FILE__, __LINE__);
        }

        // Write everything in the buffer out
        virtual void write(const data_t* buf, const size_t nbytes) {
            throw not_implemented_exception(__FILE__, __LINE__);
        };

        // Read `nbytes` at `offset` location in the file to `buf`
        virtual void read(data_t* buf, const size_t offset,
                const size_t nbytes) {
            throw not_implemented_exception(__FILE__, __LINE__);
        }

        virtual void write() {
            throw not_implemented_exception(__FILE__, __LINE__);
        }

        // Write `nbytes` at `offset` location in the file to `buf`
        virtual void write(const data_t* buf, const size_t offset,
                const size_t nbytes) {
            throw not_implemented_exception(__FILE__, __LINE__);
        }

        virtual void append(const data_t* buf, const size_t nbytes) {
            throw not_implemented_exception(__FILE__, __LINE__);
        }

        virtual void shape(const dimpair dim) {
            this->dim = dim;
        }

        virtual const dimpair& shape() {
            return this->dim;
        }

        virtual data_t* get_row(const offset_t offset) {
            throw not_implemented_exception(__FILE__, __LINE__);
        }

        virtual data_t* get_col(const offset_t offset) {
            throw not_implemented_exception(__FILE__, __LINE__);
        }

        virtual void destroy() {
            throw abstract_exception("IO::destroy");
        }
};

// Basically a matrix represented as a vector
class MemoryIO: public IO {
    public:
        MemoryIO(): IO() {
        }

        MemoryIO(data_t* data, dimpair dim,
                MAT_ORIENT orient):
            IO (dim, orient) {
            this->data = data;
        }

        void read(data_t* buf, const offset_t offset,
                const size_t nbytes=0) override {
            buf = &data[offset];
        }

        void set_data(data_t* data) {
            this->data = data;
        }

        data_t* get_data() {
            return this->data;
        }

        void write() override {
            if (this->fn.empty())
                assert(0);
            std::ofstream ofs(this->fn, std::ofstream::out);
            ofs.write(reinterpret_cast<char*>(data),
                    (dim.first*dim.second*dtype_size));
            ofs.close();
        }

        // TODO: Slow
        void transpose() override {
            data_t* tmp = new data_t[dim.first*dim.second];

            if (orientation == MAT_ORIENT::ROW) {
                for (size_t row = 0; row < dim.first; row++) {
                    for (size_t col = 0; col < dim.second; col++)
                        tmp[col*dim.first+row] = data[row*dim.second+col];
                }
                orientation = MAT_ORIENT::COL;
            } else {
                for (size_t row = 0; row < dim.first; row++) {
                    for (size_t col = 0; col < dim.second; col++)
                        tmp[col*dim.first+row] = data[row*dim.second+col];
                }
                orientation = MAT_ORIENT::ROW;
            }

            data = std::move(tmp);
            std::swap(dim.first, dim.second);
        }

        // No copying
        data_t* get_col(const offset_t offset) override {

            if (this->orientation == MAT_ORIENT::COL) {
                return &data[offset*this->dim.first];
            } else if (this->orientation == MAT_ORIENT::ROW) {
                // FIXME: Memory leak if not freed
                data_t* tmp = new data_t[dim.first];
                for (size_t row = 0; row < dim.first; row++)
                    tmp[row] = data[row*dim.second+offset];
                return tmp;
            } else {
                throw not_implemented_exception(__FILE__, __LINE__);
            }
        }

        // No copying
        data_t* get_row(const offset_t offset) override {
            if (this->orientation == MAT_ORIENT::ROW)
                return &data[offset*this->dim.second];
            else if (this->orientation == MAT_ORIENT::COL)
                throw not_implemented_exception(__FILE__, __LINE__);
            else
                throw not_implemented_exception(__FILE__, __LINE__);
        }

        static MemoryIO* cast2(IO::raw_ptr iop) {
            return static_cast<MemoryIO*>(iop);
        }

        void destroy() override {
        }
};

class SyncIO: public IO {
    private:
        std::fstream fs;

    public:
        SyncIO(): IO() {
        }

        SyncIO(const std::string fn):
            IO(fn) {
            this->fn = fn;
        }

        SyncIO(const std::string fn, dimpair dim,
                MAT_ORIENT orient) : IO (dim, orient) {
            set_fn(fn);
        }

        void set_fn(const std::string fn) override {
            this->fn = fn;
            open();
        }

        void open(const std::ios_base::openmode mode
                =std::ios_base::in | std::ios_base::out) {
            fs.open(this->fn.c_str(), mode);
            if (!fs.is_open())
                throw io_exception(std::string("Failure to open file: '")
                        + fn + std::string("'\n"));
        }

        void read(data_t* buf) override {
            if (!fs.is_open()) {
                open();
            }

            size_t size = monya::utils::get_file_size(this->fn);
            fs.read(reinterpret_cast<char*>(buf), size);
        }

        void write(const data_t* buf, const size_t nbytes) override {
            fs.write(reinterpret_cast<const char*>(buf), nbytes);
        }

        // TODO: Verify NO accidental overrite due to block size
        void write(const data_t* buf, const size_t offset,
                const size_t nbytes) override {
            fs.seekp(offset);
            write(buf, nbytes);
        }

        void append(const data_t* buf, const size_t nelem) {
            fs.write(reinterpret_cast<const char*>(buf),dtype_size*nelem);
        }

        static SyncIO* cast2(IO::raw_ptr iop) {
            return static_cast<SyncIO*>(iop);
        }

        // No copying
        data_t* get_col(const offset_t offset) override {
            assert(fs.is_open());
            if (this->orientation == MAT_ORIENT::COL) {
                fs.seekp(offset*dim.first*dtype_size);
                if (NULL == data)
                    data = new data_t[dim.first];

                fs.read(reinterpret_cast<char*>(&data[0]),
                        dtype_size*dim.first);
                return data;
            } else if (this->orientation == MAT_ORIENT::ROW) {

                // FIXME: Memory leak if not freed
                printf("WARNING: Inefficent method `get_col` for rowwise\n");
                data_t* tmp = new data_t[dim.first];
                for (size_t row = 0; row < dim.first; row++) {
                    fs.seekp((row*dim.second+offset)*dtype_size);
                    fs.read(reinterpret_cast<char*>(&tmp[row]),
                            dtype_size);
                }
                return tmp;
            } else {
                throw not_implemented_exception(__FILE__, __LINE__);
            }
        }

        // No copying
        data_t* get_row(const offset_t offset) override {
            assert(fs.is_open());

            if (this->orientation == MAT_ORIENT::ROW) {
                data_t* tmp = new data_t[dim.second];
                fs.seekp((offset*dim.second)*dtype_size);
                fs.read(reinterpret_cast<char*>(tmp), dtype_size*dim.second);
                return tmp;
            } else if (this->orientation == MAT_ORIENT::COL) {
#if 1
                // FIXME: Memory leak if not freed
                printf("WARNING: Inefficent method `get_row` for colwise\n");
                data_t* tmp = new data_t[dim.second];
                for (size_t col = 0; col < dim.second; col++) {

                    fs.seekp(((col*dim.first)+offset)*dtype_size);
                    fs.read(reinterpret_cast<char*>(&tmp[col]),
                            dtype_size);
                }
                return tmp;
#endif
                throw not_implemented_exception(__FILE__, __LINE__);
            } else {
                throw not_implemented_exception(__FILE__, __LINE__);
            }
        }

        void destroy() override {
            if (fs.is_open())
                fs.close();
            if (data != NULL)
                delete [] data;
        }
};

} } // End namespace monya::io
#endif
