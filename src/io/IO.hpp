#ifndef MONYA_I0_HPP__
#define MONYA_I0_HPP__

#include <iostream>
#include <fstream>
#include <memory>
#include "../common/exception.hpp"

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
    class IO {
        protected:
            std::string fn;

        public:
            typedef std::shared_ptr<IO<T> > ptr;
            IO() { }

            IO(const std::string fn) {
                this->fn = fn;
            }

            void set_fn(const std::string fn) {
                this->fn = fn;
            }


            const std::string get_fn() const {
                return this->fn;
            }

            // Read everything in the file
            virtual void read(T* buf) {
                throw not_implemented_exception();
            }

            // Write everything in the buffer out
            virtual void write(const T* buf) {
                throw not_implemented_exception();
            }

            // Read `nbytes` at `offset` location in the file to `buf`
            virtual void read(T* buf, const size_t offset,
                    const size_t nbytes) {
                throw not_implemented_exception();
            }

            // Write `nbytes` at `offset` location in the file to `buf`
            virtual void write(const T* buf, const size_t offset,
                    const size_t nbytes) {
                throw not_implemented_exception();
            }

            virtual void append(const T* buf, const size_t nbytes) {
                throw not_implemented_exception();
            }
    };

template <typename T>
    class SyncIO: IO<T> {
        private:
            std::fstream fs;

        public:
            static typename IO<T>::ptr create() {
                return IO<T>::ptr(new SyncIO<T>());
            }

            static typename IO<T>::ptr create(const std::string fn) {
                return IO<T>::ptr(new SyncIO<T>(fn));
            }

            SyncIO(const std::string fn) : IO<T>(fn) {
            }

            void open(const std::ios_base::openmode mode
                    =std::ios_base::in | std::ios_base::out) {
                fs.open(this->fn.str(), mode);
                assert(fs.is_open());
            }

            void read(T* buf) override {
                std::streampos size = fs.tellg();
                fs.seekg(0, std::ios::beg);
                fs.read(reinterpret_cast<char*>(buf), size);
            }

            // TODO: Verify accidental overrite due to block size
            void write(const T* buf, const size_t offset,
                    const size_t nbytes) override {
                fs.seekp(offset);
                fs.write(reinterpret_cast<const char*>(buf), nbytes);
            }

            void append(const T* buf, const size_t nbytes) {
                fs.write(reinterpret_cast<const char*>(buf), nbytes);
            }

            static std::shared_ptr<SyncIO<T> > cast2(typename IO<T>::ptr iop) {
                return std::static_pointer_cast<SyncIO<T> >(iop);
            }

            ~SyncIO() {
                if (fs.is_open())
                    fs.close();
            }
    };

} } // End namespace monya::io

#endif
