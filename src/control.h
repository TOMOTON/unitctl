
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */

#ifndef CONTROL_H
#define CONTROL_H


int await_unitd(char const *unix_socket_path);

int configure_unitd(char const *unix_socket_path, char const *request_target, char const *path);


#endif // CONTROL_H