
install fedora packages:
sudo dnf install ffmpeg ffmpeg-free-devel g++ make cmake --skip-unavailable -y

make sure to clone this repo with 
git clone --recursive (repo url)
to include sdl3 in the repo, 
if you already cloned it, just run:
git submodule update --init --recursive

