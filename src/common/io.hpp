#ifndef UTILS_I0_HPP__
#define UTILS_I0_HPP__

#include <vector>
#include <iostream>
#include <fstream>

namespace monya {

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
        class reader {
            private:
                std::string fn;

            protected:
                std::ifstream f;
                size_t ncol, nrow;

            public:
                reader(const std::string fn) {
                    this->fn = fn;
                    ncol = 0;
                    nrow = 0;
                }

                virtual void read(std::vector<T>& data) = 0;
                virtual bool readline(std::vector<T>& data) = 0;
                virtual void open() = 0;

                const std::string get_fn() const {
                    return this->fn;
                }

                void set_fn(const std::string fn) {
                    this->fn = fn;
                }

                const size_t get_nrow() const {
                    return nrow;
                }

                const size_t get_ncol() const {
                    return ncol;
                }

                void set_nrow(const size_t nrow) {
                    this->nrow = nrow;
                }

                void set_ncol(const size_t ncol) {
                    this->ncol = ncol;
                }

                ~reader() {
                    f.close();
                }
        };

    template <typename T>
        class bin_rm_reader : public reader<T> {
            public:
                bin_rm_reader(const std::string fn) : reader<T>(fn) {
                    this->open();
                }

                void seek(const size_t nbytes) {
                    this->f.seekg(nbytes);
                }

                void read(std::vector<T>& data) override {
                    assert(data.size() > 0);
                    this->f.read(reinterpret_cast<char*>(data.data()),
                            data.size()*sizeof(T));
                }

                bool readline(std::vector<T>& data) override {
                    if (this->ncol) {
                        if (this->f.read(reinterpret_cast<char*>(data.data()),
                                    data.size()*sizeof(T))) {
                            this->nrow++;
                            return true;
                        }
                    } else {
#ifndef BIND
                        std::cout << "ncol: " << this->ncol << "\n";
                        fprintf(stderr, "Cannot read a line without `ncol`\n");
#endif
                        assert(false);
                    }
                    return false;
                }

                void open() override {
                    this->f.open(this->get_fn(), std::ios::in | std::ios::binary);
                    assert(this->f.good());
                }
        };
}

#endif
