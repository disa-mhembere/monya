/*
 * Copyright 2017 Neurodata (https://neurodata.io)
 * Written by Disa Mhembere (disa@cs.jhu.edu)
 *
 * This file is part of Monya.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MONYA_I0_HPP__
#define MONYA_I0_HPP__

#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <utility>

#include "../common/exception.hpp"
#include "../common/types.hpp"
#include "vecs_reader.hpp"

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

static inline size_t get_file_size(std::string filename) {
        struct stat stat_buf;
        int rc = stat(filename.c_str(), &stat_buf);
        return rc == 0 ? stat_buf.st_size : -1;
}

static inline std::string get_file_ext(const std::string& s) {
    size_t i = s.rfind('.', s.length());

    if (i != std::string::npos) {
        return (s.substr(i + 1, s.length() - i));
    }
    return "";
}

class IO {
    protected:
        std::string fn;
        size_t dtype_size;
        data_t* data;
        orient_t orientation;
        dimpair dim;

    public:
        typedef IO* raw_ptr;

        IO() : fn(""), dtype_size(sizeof(data_t)), data(NULL),
                orientation(INVALID), dim(dimpair(0,0)) {
        }

        virtual void transpose() {
            throw not_implemented_exception(__FILE__, __LINE__);
        }

        virtual void print() {
            printf("fn: %s\n dypte size: %lu, orientation: %s, shape:"
                " (%lu, %lu)\n", fn.c_str(), dtype_size,
                (orientation == orient_t::COL ? "COL" : "ROW"),
                 dim.first, dim.second);
        }

        IO(const std::string fn): IO() {
            this->fn = fn;
        }

        IO(const dimpair dim, orient_t orient): IO() {
            this->dim = dim;
            this->orientation = orient;
        }

        virtual void set_orientation(const orient_t orient) {
            this->orientation = orient;
        }

        const orient_t get_orientation() const {
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
class MemoryIO;
static MemoryIO* memIOSingleton = NULL;

// Singleton, but not thread safe. Called serially by `TreeProgram`s
class MemoryIO: public IO {
    private:
        bool is_data_local;

    public:
        static MemoryIO* create() {
            if (!memIOSingleton)
                memIOSingleton = new MemoryIO;
            return memIOSingleton;
        }

        static MemoryIO* create(data_t* data, dimpair dim,
                orient_t orient) {
            if (!memIOSingleton)
                memIOSingleton = new MemoryIO(data, dim, orient);
            return memIOSingleton;
        }

        MemoryIO(): IO() {
            is_data_local = false;
        }

        MemoryIO(data_t* data, dimpair _dim,
                orient_t orient): IO (_dim, orient) {
            this->data = data;
        }

        void set_fn(const std::string fn) override {
            if (this->fn.empty())
                IO::set_fn(fn);
        }

        void set_orientation(const orient_t orient) override {
            if (this->orientation == orient_t::INVALID)
                IO::set_orientation(orient);
            // else do nothing
        }

        void shape(const dimpair dim) override {
            if (this->dim == dimpair(0,0))
                IO::shape(dim);
            // else do nothing
        }

        void from_file() {
            if (NULL != data) return;

            assert (!this->fn.empty());
            if (NULL == data)
                data = new data_t[dim.first*dim.second];

            // TODO: Use an enum for filetype
            if (get_file_ext(fn) == "fvecs") {
                vecs_reader* vr = new fvecs_reader(fn, dim.first,
                        dim.second);
                vr->read(data);
                delete vr;
            } else {
                std::fstream fs;
                fs.open(this->fn.c_str(), std::ios_base::in);
                if (!fs.is_open())
                    throw io_exception(std::string("Failure to open file: '")
                            + fn + std::string("'\n"));

                fs.read(reinterpret_cast<char*>(&data[0]),
                        dim.first*dim.second*dtype_size);
            }
            is_data_local = true;
#if 0
            printf("Echoing 2 samples of the dataset:\n");
            print_mat(&data[0], 2, dim.second);
#endif
        }

        void read(data_t* buf, const offset_t offset,
                const size_t nbytes=0) override {
            buf = &data[offset];
        }

        void set_data(data_t* data) {
            if (this->data) return;
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

            if (orientation == orient_t::ROW) {
                for (size_t row = 0; row < dim.first; row++) {
                    for (size_t col = 0; col < dim.second; col++)
                        tmp[col*dim.first+row] = data[row*dim.second+col];
                }
                orientation = orient_t::COL;
            } else {
                for (size_t row = 0; row < dim.first; row++) {
                    for (size_t col = 0; col < dim.second; col++)
                        tmp[col*dim.first+row] = data[row*dim.second+col];
                }
                orientation = orient_t::ROW;
            }

            data = std::move(tmp);
            std::swap(dim.first, dim.second);
        }

        data_t* get_col(const offset_t offset) override {

            if (this->orientation == orient_t::COL) {
                return &data[offset*this->dim.first];
            } else if (this->orientation == orient_t::ROW) {
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
            if (this->orientation == orient_t::ROW) {
                return &data[offset*this->dim.second];
            } else if (this->orientation == orient_t::COL) {
                // FIXME: Memory leak if not freed
                data_t* tmp = new data_t[dim.second];
                for (size_t col = 0; col < dim.second; col++)
                    tmp[col] = data[col*dim.first+offset];
                return tmp;
            } else {
                throw not_implemented_exception(__FILE__, __LINE__);
            }
        }

        static MemoryIO* cast2(IO::raw_ptr iop) {
            return static_cast<MemoryIO*>(iop);
        }

        void destroy() override {
            if (NULL != data && is_data_local) {
                delete [] data;
                data = NULL;
            }
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
                orient_t orient) : IO (dim, orient) {
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

            size_t size = get_file_size(this->fn);
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

        void append(const data_t* buf, const size_t nelem) override {
            fs.write(reinterpret_cast<const char*>(buf),dtype_size*nelem);
        }

        static SyncIO* cast2(IO::raw_ptr iop) {
            return static_cast<SyncIO*>(iop);
        }

        // No copying
        data_t* get_col(const offset_t offset) override {
            assert(fs.is_open());
            if (this->orientation == orient_t::COL) {
                fs.seekp(offset*dim.first*dtype_size);
                if (NULL == data)
                    data = new data_t[dim.first];

                fs.read(reinterpret_cast<char*>(&data[0]),
                        dtype_size*dim.first);
#if 0
                std::string str_col = "Echo of col retrieved: \n[ ";
                for (size_t i = 0; i < dim.first; i++) {
                    str_col += std::to_string(data[i]) + std::string(" ");
                }
                str_col += std::string("]");
                printf("%s\n", str_col.c_str());
#endif
                return data;
            } else if (this->orientation == orient_t::ROW) {
                // FIXME: Memory leak if not freed
                printf("WARNING: Inefficient method `get_col` for rowwise\n");
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

            if (this->orientation == orient_t::ROW) {
                data_t* tmp = new data_t[dim.second];
                fs.seekp((offset*dim.second)*dtype_size);
                fs.read(reinterpret_cast<char*>(tmp), dtype_size*dim.second);
                return tmp;
            } else if (this->orientation == orient_t::COL) {
#if 1
                // FIXME: Memory leak if not freed
                printf("WARNING: Inefficient method `get_row` for colwise\n");
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
