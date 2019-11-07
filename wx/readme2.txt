# need to install the following packages(s):
# libwxgtk3.0-dev - wxWidgets Cross-platform C++ GUI toolkit (GTK+ development)

g++ `wx-config --cxxflags` -DwxUSE_GRID bball.cpp -o bball `wx-config --libs`
