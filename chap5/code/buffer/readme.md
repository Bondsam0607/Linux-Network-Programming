The program is to test the usage of SO_RCVBUF and SO_SNDBUF

In raspberry system, the minimum sender buffer size is 4480, if you set it < 2240, it will be 4480, if > 2240, it will be 2*your option.

In raspberry system, the minimum receiver buffer size is 2240, if you set it < 1120, it will be 2240, if > 1120, it will be 2*your option.

In macOS, there's no minimum.
