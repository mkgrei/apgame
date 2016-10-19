FROM pritunl/archlinux:latest

RUN yes | pacman -Syu && \
    yes | pacman -S gcc boost git vim openssh make cmake

RUN cd ~/ && \
    curl https://raw.githubusercontent.com/Shougo/neobundle.vim/master/bin/install.sh > install.sh && \
    sh install.sh && \
    git clone https://github.com/kktk-KO/vim && \
    cp vim/.vimrc .
