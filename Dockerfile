FROM ubuntu:16.04
MAINTAINER  Disa Mhembere <disa@cs.jhu.edu>

RUN apt-get update && apt-get install -y openssh-server

RUN echo "deb http://cran.rstudio.com/bin/linux/ubuntu xenial/" >> /etc/apt/sources.list
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E084DAB9
RUN apt-get update
RUN apt-get install -y git cmake g++
RUN apt-get install -y libboost-dev libboost-system-dev libboost-filesystem-dev libaio-dev libatlas-base-dev zlib1g-dev
RUN apt-get install -y numactl xfsprogs
RUN apt-get install -y libnuma-dev libhwloc-dev
RUN apt-get install -y wget vim

# Dev environment
RUN mkdir Research
WORKDIR Research
RUN printf "%s\n\t%s\n%s\n%s" "function parse_git_branch_and_add_brackets {" \
    "git branch --no-color 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\ \[\1\]/'" \
"}" \
"PS1=\"\h:\W \u\[\033[0;32m\]\$(parse_git_branch_and_add_brackets) \[\033[0m\]\$ \"" >> ~/.bashrc
RUN git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim
RUN wget https://gist.githubusercontent.com/disa-mhembere/1baf10b1c3b1e047750538f9a1d3eb3c/raw/04046a3090651fa92dd691d6ba411113bb689127/.vimrc -O ~/.vimrc
RUN git clone https://github.com/disa-mhembere/matree.git

#RUN
#echo ":PluginInstall" | vim

ENTRYPOINT ["bash"]
