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
