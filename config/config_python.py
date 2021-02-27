#!/usr/bin/env python3

import argparse
from distutils import sysconfig


def main():
    parser = argparse.ArgumentParser(description="Prints specified library directory")
    parser.add_argument(
        "libdir",
        choices=["archlib", "archsitelib", "lib", "sitelib"],
        action="store",
        help="Library directory path to get",
    )
    args = parser.parse_args()
    if args.libdir == "archlib":
        print(sysconfig.get_python_lib(1, 1))
    elif args.libdir == "lib":
        print(sysconfig.get_python_lib(0, 1))
    elif args.libdir == "archsitelib":
        print(sysconfig.get_python_lib(1, 0))
    elif args.libdir == "sitelib":
        print(sysconfig.get_python_lib(0, 0))


if __name__ == "__main__":
    main()
