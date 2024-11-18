# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Set the working directory inside the container
WORKDIR /app

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    automake \
    autoconf \
    libtool \
    valgrind \
    m4 \
    build-essential \
    git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Copy the repository into the /app directory
COPY . /app

# Make sure the script is executable
RUN chmod +x /app/make_all.sh
<<<<<<< HEAD
RUN /app/make_all.sh


# Run the make_all.sh script
CMD ["/bin/bash"]
#CMD ["/bin/bash", "-c", "/app/make_all.sh && exec /bin/bash"]
=======

# Run the make_all.sh script
CMD ["/bin/bash"]
# CMD ["/bin/bash", "-c", "/app/make_all.sh && exec /bin/bash"]
>>>>>>> baf404c (Included Dockerfile, made poptrie have command line arguments)
