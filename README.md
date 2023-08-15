# Subnetting Library

A C library for subnetting calculations and IP address manipulation.
Works for Subnet Masks from /24 to /32. Relies on bitwise operations for speed and efficiency.

## Overview

The Subnetting Library provides functions for subnetting calculations, IP address manipulation, and generating subnet information tables. It offers various features to simplify subnetting tasks, including:

- Creating subnets from IP addresses and prefix lengths.
- Classifying IP addresses into Class A, B, C, D, or E.
- Calculating assignable IP address ranges within a subnet.
- Converting IP addresses to different formats.
- Generating subnet information tables.

## Installation

Clone this repository and include the `subnet.h` header file in your project.

```c
#include "subnet.h"
```

## Usage

```c

#include "subnet.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  // Create a subnet with IP address 192.168.1.0 and prefix length 24
  Subnet subnet = create_subnet(0xC0A80100U, 24);

  char *subnetIPString = to_ipv4(subnet.ip).addr;
  printf("Subnet IP: %s\n", subnetIPString);

  // Classify an IP address
  IPAddress ipToClassify = {0xC0A80101U, 0}; // IP: 192.168.1.1
  char ipClass = classify_ip(ipToClassify);

  char *ipToClassifyString = to_ipv4(ipToClassify.ip).addr;
  printf("IP Address: %s is in class %c\n", ipToClassifyString, ipClass);

  // Compute the assignable IP range
  IPAddress rangeStart, rangeEnd;
  assignable_range(subnet, &rangeStart, &rangeEnd);

  char *rangeStartString = to_ipv4(rangeStart.ip).addr;
  char *rangeEndString = to_ipv4(rangeEnd.ip).addr;

  printf("Assignable IP Range: %s - %s\n", rangeStartString, rangeEndString);
  printf("Total Assignable: %u\n", compute_assignable_addresses(subnet));

  // Create subnet from string
  const char *subnet_string = "192.168.4.192/26";
  subnet = create_subnet_from_string(subnet_string);

  char *addr = to_ipv4(subnet.ip).addr;
  char *mask = to_ipv4(subnet.mask).addr;

  printf("IP address: %s\n", addr);
  printf("Mask address: %s\n", mask);
  print_ips_in_subnet(subnet);

  size_t num_ips;
  IPV4 *ip_addresses = get_ips_in_subnet(subnet, &num_ips);

  printf("IP addresses in the subnet are: %zu\n", num_ips);
  for (size_t i = 0; i < num_ips; i++) {
    printf("%s\n", ip_addresses[i].addr);
  }
  free(ip_addresses);

  // Generate subnet table
  printf("\n\nNetwork ID: 192.168.4.0/24\n");
  printf("Table for creation of 4 subnets:\n");

  SubnetInfo *subnet_info = get_subnet_table("192.168.4.0/24", 4);

  printf("\n\nNew Subnet Mask: %s\n", subnet_info->subnet_mask);
  print_subnet_table(subnet_info, 4);
  free(subnet_info);

  return 0;
}

```

Compile and run example:

```bash
gcc -Wall -Werror -pedantic -ggdb -fsanitize=address main.c subnet.c && ./a.out
```
