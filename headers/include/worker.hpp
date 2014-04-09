// Chlorine Main Header
#ifndef CHLORINE_WORKER
#define CHLORINE_WORKER

namespace ch
{
    class Worker
    {
    public:

        // Constructors
        Worker(); // Default Constructor

        // Proposed Class Methods
        void set_platform(unsigned int platform);
        void set_device(unsigned int device);
        void set_kernel(std::string kernel_source);
        template<typename T>
        void push(std::vector<T> v);
        template<typename V,typename ...Args>
        void push(V vec,Args ...a);
        std::vector<cl::Buffer> buffers;

    private:

        cl::CommandQueue mQueue;
        cl::Context      mContext;
        cl::Device       mDevice;
        cl::Platform     mPlatform;
        cl::Program      mProgram;

    };

    // Default Constructor
    Worker::Worker()
    {
        set_platform(0);
        set_device(0);
        set_kernel("default.cl");

        std::cout << mPlatform.getInfo<CL_PLATFORM_NAME>() << std::endl;
        std::cout << mDevice.getInfo<CL_DEVICE_NAME>() << std::endl;
        std::cout << "Hello Busybot!\n";
    }

    void Worker::set_platform(unsigned int platform)
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(& platforms);
        mPlatform = platforms[platform];
    }

    void Worker::set_device(unsigned int device)
    {
        std::vector<cl::Device> devices;
        mPlatform.getDevices(CL_DEVICE_TYPE_DEFAULT, & devices);
        mContext = cl::Context(devices);
        mDevice = devices[device];
        mQueue = cl::CommandQueue(mContext, mDevice, CL_QUEUE_PROFILING_ENABLE);
    }

    template<typename T>
    void Worker::push(std::vector<T> vec) {

       unsigned int array_size = vec.size()*sizeof(T);

       buffers.push_back(cl::Buffer(mContext,
            CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
            array_size, & vec.front()));
    }

    template <typename V, typename ...Args>
    void Worker::push(V vec, Args... args) {
        push(vec);
        push(args...);
    }

    void Worker::set_kernel(std::string kernel_source)
    {
        // Read Contents of Kernel
        std::ifstream fd(kernel_source);
        std::string kernel(std::istreambuf_iterator<char>(fd),
                          (std::istreambuf_iterator<char>()));

        // Build Kernel Using the Current Context
        cl::Program::Sources source(1, std::make_pair(
            kernel.c_str(), kernel.length()+1));
        mProgram = cl::Program(mContext, source);
        mProgram.build(mContext.getInfo<CL_CONTEXT_DEVICES>());
    }
}

#endif