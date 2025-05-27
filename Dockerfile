FROM ubuntu:22.04 AS builder

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        ffmpeg \
        git \
        pkg-config \
        gcc \
        libavcodec-dev \
        libavformat-dev \
        libavutil-dev \
        libswscale-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /ffmpeg

COPY main.c .

RUN gcc -o trim main.c -lavcodec -lavutil -lavformat -lswscale && \
    chmod +x trim && \
    ls -l trim


FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ffmpeg \
        libavcodec-dev \
        libavformat-dev \
        libavutil-dev \
        libswscale-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN mkdir -p /app/input /app/output

COPY --from=builder /ffmpeg/trim /app/trim
RUN chmod +x /app/trim && \
    ls -l /app/trim

VOLUME ["/app/input", "/app/output"]

CMD ["/app/trim"]

