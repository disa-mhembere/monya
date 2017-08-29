#include <boost/foreach.hpp>
#include "io_interface.h"

static size_t NCOL = 512;
static size_t NROW = 64;

class compute_callback: public safs::callback {
    public:
        virtual int invoke(safs::io_request *reqs[], int num);
};

static void run_computation(char* buf, size_t bufsize) {
    double* arr = reinterpret_cast<double*>(buf);

    for (size_t i = 0; i < 5; i++)
        std::cout << arr[i] << " ";
    std::cout << "...\n";
}

int compute_callback::invoke(safs::io_request *reqs[], int num) {
    for (int i = 0; i < num; i++) {
        char *buf = reqs[i]->get_buf();
        // io_request has file_id and offset
        // also buf address can be used
        run_computation(buf, reqs[i]->get_size());
        free(buf);
    }
    return 0;
}


// We will use the 50 x 5 matrix
class task {
private:
    size_t ncol;
    size_t nrow;
    size_t taskno;

public:
    task(size_t taskno) {
        this->taskno = taskno;
        ncol = NCOL;
        nrow = NROW;
    }

    size_t get_size() const {
        return sizeof(double)*ncol;
    }

    off_t get_offset() const {
        return taskno*ncol;
    }

    const size_t get_taskno() const {
        return this->taskno;
    }
};

static void test(const std::string conf_file, const std::string &graph_file,
        const std::vector<task> &tasks) {
    config_map::ptr configs = config_map::create(conf_file);
    safs::init_io_system(configs);
    safs::file_io_factory::shared_ptr factory = safs::create_io_factory(graph_file,
            safs::REMOTE_ACCESS);
    safs::io_interface::ptr io = safs::create_io(factory,
            thread::get_curr_thread());
    io->set_callback(safs::callback::ptr(new compute_callback()));

    // How to choose max_ios threshold? Maybe 100 KB. For 4K,8K, 16K io
    //  requests use 64 pending io request. Larger like 1MB - 16 MB we can
    //  use like 8 pending ios.

    int max_ios = 20;
    BOOST_FOREACH(task t, tasks) {

        while (io->num_pending_ios() >= max_ios) {
            printf("\n... Waiting for ios to complete!\n");
            io->wait4complete(1);
        }

        // This is directed I/O. Memory buffer, I/O offset and I/O size
        // all need to be aligned to the I/O block size.
        size_t io_size = ROUNDUP_PAGE(t.get_size());

        printf("Task: %lu, io_size is: %lu\n", t.get_taskno(), io_size);

        safs::data_loc_t loc(factory->get_file_id(), t.get_offset());
        // The buffer will be free'd in the callback function.
        char *buf = (char *) valloc(io_size);
        safs::io_request req(buf, loc, io_size, READ);
        io->access(&req, 1);
    }
    io->wait4complete(io->num_pending_ios());
}

int main(int argc, char* argv[]) {
    std::vector<task> tasks;
    for (size_t i = 0; i < 50; i ++)
        tasks.push_back(task(i));

    test("/home/disa/Sandbox/SAFS/conf/run_graph.txt",
            "mat_64x512.bin", tasks);
    return EXIT_SUCCESS;
}
