all: webcam_server

CXXFLAGS=	-c -g -O0 -fPIC -lpthread

OBJS_SERVER=	capture.o vcompress.o socket.o server.o
LIBS_SERVER=	-lavcodec -lswscale -lavutil -lx264 -lpthread

.cpp.o:
	$(CXX) $(CXXFLAGS) $<

webcam_server: $(OBJS_SERVER)
	$(CXX) -o $@ $^ $(LIBS_SERVER)
clean:
	rm -f *.o
	rm -f webcam_server
