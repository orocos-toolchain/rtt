noinst_LIBRARIES = libkrypton_handler.a libkrypton.a
noinst_PROGRAMS = linux_client_for_rt_socket krypton_socketserver_lxrt

linux_client_for_rt_socket_SOURCES = linux_client_for_rt_socket.c
linux_client_for_rt_socket_CFLAGS = \
        -I$(top_srcdir)/include \
	-I$(top_builddir)/include

krypton_socketserver_lxrt_SOURCES = krypton_socketserver_lxrt.c
krypton_socketserver_lxrt_CFLAGS = \
        -I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	@LXRT_CFLAGS@

libkrypton_a_SOURCES =  krypton_handler.c
libkrypton_a_CFLAGS = \
	-I$(top_srcdir)/include \
	@RTAI_KMOD_CFLAGS@

libkrypton_handler_a_SOURCES = krypton_handler.c
libkrypton_handler_a_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	@RTAI_KMOD_CFLAGS@

krypton_handler.o: libkrypton_handler.a
	$(LD) --whole-archive $< -r -o $@

all-local: krypton_handler.o linux_client_for_rt_socket krypton_socketserver_lxrt

