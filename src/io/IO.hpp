#ifndef MONYA_I0_HPP__
#define MONYA_I0_HPP__

#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>

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
        char* data; // To avoid templating
        MAT_ORIENT orientation;
        dimpair dim;

    public:
        typedef IO* raw_ptr;

        IO() {
            data = NULL;
            fn = "";
            orientation = MAT_ORIENT::INVALID;
        }

        IO(const std::string fn, const size_t dtype_size): IO() {
            this->fn = fn;
            this->dtype_size = dtype_size;
        }

        IO(const dimpair dim, MAT_ORIENT orient, const size_t dtype_size): IO() {
            this->dim = dim;
            this->orientation = orient;
            this->dtype_size = dtype_size;
        }

        void set_orientation(const MAT_ORIENT orient) {
            this->orientation = orient;
        }

        const MAT_ORIENT get_orientation() const {
            return this->orientation;
        }

        void set_dtype_size(size_t dtype_size) {
            this->dtype_size = dtype_size;
        }

        void set_fn(const std::string fn) {
            this->fn = fn;
        }

        const std::string get_fn() const {
            return this->fn;
        }

        // Read everything in the file
        virtual void read(void* buf) {
            throw not_implemented_exception();
        }

        // Write everything in the buffer out
        virtual void write(const void* buf, const size_t nbytes) {
            throw not_implemented_exception();
        };

        // Read `nbytes` at `offset` location in the file to `buf`
        virtual void read(void* buf, const size_t offset,
                const size_t nbytes) {
            throw not_implemented_exception();
        }

        virtual void write() {
            throw not_implemented_exception();
        }

        // Write `nbytes` at `offset` location in the file to `buf`
        virtual void write(const void* buf, const size_t offset,
                const size_t nbytes) {
            throw not_implemented_exception();
        }

        virtual void append(const void* buf, const size_t nbytes) {
            throw not_implemented_exception();
        }

        virtual void shape(const dimpair dim) {
            throw not_implemented_exception();
        }

        virtual const dimpair& shape() {
            throw not_implemented_exception();
        }

        virtual void* get_row(const offset_t offset) {
            throw not_implemented_exception();
        }

        virtual void* get_col(const offset_t offset) {
            throw not_implemented_exception();
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

        MemoryIO(void* data, dimpair dim,
                MAT_ORIENT orient, size_t dtype_size):
            IO (dim, orient, dtype_size) {
            this->data = reinterpret_cast<char*>(data);
        }

        // Avoid template by using void and specifying datatype size
        //  for reads.
        void read(void* buf, const offset_t offset,
                const size_t nbytes=0) override {
            buf = data + (dtype_size*offset);
        }

        void shape(dimpair dim) override {
            this->dim = dim;
        }

        const dimpair& shape() override {
            return this->dim;
        }

        void set_data(void* data) {
            this->data = reinterpret_cast<char*>(data);
        }

        void* get_data() {
            return this->data;
        }

        void write() override {
            if (this->fn.empty())
                assert(0);
            std::ofstream ofs(this->fn, std::ofstream::out);
            ofs.write(data, (dim.first*dim.second*dtype_size));
            ofs.close();
        }

        // No copying
        void* get_col(const offset_t offset) override {
            if (this->orientation == MAT_ORIENT::COL) {
                return &data[(dtype_size*offset*this->dim.first)];
            }
            else if (this->orientation == MAT_ORIENT::ROW)
                throw not_implemented_exception();
            else
                throw not_implemented_exception();
        }

        // No copying
        void* get_row(const offset_t offset) override {
            if (this->orientation == MAT_ORIENT::ROW)
                return &data[(dtype_size*offset*this->dim.second)];
            else if (this->orientation == MAT_ORIENT::COL)
                throw not_implemented_exception();
            else
                throw not_implemented_exception();
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

        SyncIO(const std::string fn, const size_t dtype_size):
            IO(fn, dtype_size) {
        }

        SyncIO(const std::string fn, dimpair dim,
                MAT_ORIENT orient, size_t dtype_size) :
            IO (dim, orient, dtype_size) {
            this->fn = fn;
            this->data = NULL;
        }

        void open(const std::ios_base::openmode mode
                =std::ios_base::in | std::ios_base::out) {
            fs.open(this->fn.c_str(), mode);
            assert(fs.is_open());
        }

        void read(void* buf) override {
            if (!fs.is_open()) {
                open();
            }

            size_t size = monya::utils::get_file_size(this->fn);
            fs.read(reinterpret_cast<char*>(buf), size);
        }

        void write(const void* buf, const size_t nbytes) override {
            fs.write(reinterpret_cast<const char*>(buf), nbytes);
        }

        // TODO: Verify NO accidental overrite due to block size
        void write(const void* buf, const size_t offset,
                const size_t nbytes) override {
            fs.seekp(offset);
            write(buf, nbytes);
        }

        void append(const void* buf, const size_t nbytes) {
            fs.write(reinterpret_cast<const char*>(buf), nbytes);
        }

        static SyncIO* cast2(IO::raw_ptr iop) {
            return static_cast<SyncIO*>(iop);
        }

        // No copying
        void* get_col(const offset_t offset) override {
            if (this->orientation == MAT_ORIENT::COL) {
                fs.seekp(offset*dim.first*dtype_size);
                if (NULL == data)
                    data = new char[dtype_size*dim.first];

                fs.read(&data[0], dtype_size*dim.first);
                return data;
            } else if (this->orientation == MAT_ORIENT::ROW)
                throw not_implemented_exception();
            else
                throw not_implemented_exception();
        }

        // No copying
        void* get_row(const offset_t offset) override {
            if (this->orientation == MAT_ORIENT::ROW)
                throw not_implemented_exception();
            else if (this->orientation == MAT_ORIENT::COL)
                throw not_implemented_exception();
            else
                throw not_implemented_exception();
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
