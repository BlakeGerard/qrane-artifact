FROM ubuntu:18.04
SHELL ["/bin/bash", "-c"]

# tzdata configuration 
ENV TZ=US/Central
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Basic package installation
RUN apt-get update -y && apt install -y
RUN apt install -y nano
RUN apt install -y flex
RUN apt install -y bison
RUN apt install -y build-essential
RUN apt install -y libgmp3-dev
RUN apt install -y zlib1g
RUN apt install -y libntl35

# Python3.9 installation
RUN apt-get install -y software-properties-common
RUN add-apt-repository ppa:deadsnakes/ppa -y
RUN apt update -y
RUN apt install -y python3.9
RUN apt install -y python3-pip
RUN apt install -y python3.9-dev
RUN apt install -y python3.9-distutils

# Python packages for qrane
RUN python3.9 -m pip install --upgrade pip
RUN python3.9 -m pip install --upgrade numpy networkx pandas matplotlib pytket pebble Pillow more_itertools

COPY ./ /home/

# Install qrane
#WORKDIR /home/qrane
#RUN ./qrane-install.sh --install-barvinok && source ./qrane-vars.sh && make

CMD /bin/bash
