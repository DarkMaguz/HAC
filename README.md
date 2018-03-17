# HAC
### Hash Algorithm Collider

This is an old project of mine created to test a hash algorithm I created for fun back in the mid 00’s playing around with C/C++. This was mainly intented as an exorcise in creating multithreaded network applications. The hash algorithm it self is an 1684 bit hash algorithm with cryptographic properties, but is not to be taken seriously.

The server app has been dockerized, so it’s easy to run.

### Running the server
```bash
export MYSQL_USER=hac
export MYSQL_PASSWORD=cah
export GMAIL_USER=my.gmail.address@gmail.com
export GMAIL_PASSWORD=some.password
./run-server.sh
```

### Running the client
First register a new user:
```bash
./hac -r foobar foo@bar.com
```
Activate the account:
```bash
./hac -a
```
Run the client:
```bash
./hac
```

### Plans for the future
- [x] Initial clean up. (Completed Marts 2018)
- [ ] Add unit testing. (In progress)
- [ ] Create a web interface for statistics.
- [ ] Separate the algorithm from the client/server, so that this project can be used for other purposes.
- [ ] Make the server scalable.
