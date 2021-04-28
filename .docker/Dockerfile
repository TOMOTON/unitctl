ARG DEBIAN_VERSION=stretch


#--
FROM debian:${DEBIAN_VERSION} as builder

RUN apt-get -y update \
        && apt-get -y install build-essential apt-transport-https ca-certificates gnupg curl

WORKDIR /work
COPY Makefile /work/
COPY src /work/src

RUN echo "Building unitctl..." \
        && mkdir lib bin \
        && make

RUN echo "Downloading Nginx Unit installation package..." \
        && export APT_KEY_DONT_WARN_ON_DANGEROUS_USAGE=1 \
        && curl -sL https://nginx.org/keys/nginx_signing.key | apt-key add - \
        && echo "deb https://packages.nginx.org/unit/debian/ stretch unit\ndeb-src https://packages.nginx.org/unit/debian/ stretch unit\n" > /etc/apt/sources.list.d/unit.list \
        && apt-get -y update \
        && chown -R _apt:root . \
        && apt-get -yq download unit \
        && ls -Al


#--
FROM debian:${DEBIAN_VERSION}

COPY --from=builder /work/*.deb /tmp
RUN apt-get -y update \
        && apt-get install -y libssl1.1 \
        && dpkg -i /tmp/*.deb

RUN echo "New user vcap (2000)" \
        && groupadd --gid 2000 vcap \
        && useradd --uid 2000 -g vcap vcap \
        && mkdir /home/vcap \
        && chown vcap:vcap /home/vcap

USER vcap

WORKDIR /home/vcap
COPY --chown=vcap:vcap .data/www /home/vcap/www
COPY --chown=vcap:vcap .data/vcap/config.json /home/vcap
COPY --from=builder --chown=vcap:vcap /work/bin/unitctl /home/vcap

ENTRYPOINT ["/home/vcap/unitctl", "config.json"]