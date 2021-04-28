# Unit Supervisor

(or Unit Control unitctl)

## Building

```
make
```

## Running

```
./bin/unitctl .data/config.json
```

```
sudo /usr/sbin/unitd --no-daemon --control unix:/var/run/control.unit.sock
```

## Acknowledgements

This project was started with the C template provided by Gustavo Pantuza Coelho
Pinto.

## Packaging

### Docker

Build image:
```
cd .docker
sudo docker image build -f .docker/Dockerfile -t docker-registry.tomoton.com:443/unitctlstatic .
```

Inspect file system, not running:
```
sudo docker run -it --entrypoint /bin/sh docker-registry.tomoton.com:443/unitctlstatic
```

Shell into running:
```
sudo docker exec -it UNIT /bin/sh
```

Run image:
```
sudo docker rm UNIT && sudo docker run -it -p 9090:9090 --name UNIT docker-registry.tomoton.com:443/unitctlstatic
```
