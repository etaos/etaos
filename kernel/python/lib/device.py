#
#   ETA/OS - Device base class
#   Copyright (C) 2017  Michel Megens <dev@bietje.net>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published
#    by the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

## @addtogroup python-device
# @{

## @package Device
#  @brief Provides PyMite's device module

"""__NATIVE__
#include <etaos/stdio.h>
#include <etaos/device.h>
#include <etaos/vfs.h>
#include <etaos/unistd.h>
#include <etaos/error.h>
"""

__name__ = "device"

## Python device class
class Device(object):
        ## Create a new device object.
        # @param name Device name.
        def __init__(self, name):
                self.path = "/dev/%s" % name

        ## Read a string of bytes from a device.
        # @param num Number of bytes to read.
        # @param addr Address to start reading at.
        # @return Bytes read from \p this device in a string object.
        def read(self, num, addr = None):
                return dev_read(self.path, num, addr)

        ## Write a string to a device.
        # @param data to write.
        # @param addr Address to start writing at.
        # @return An error code
        def write(self, data, addr = None):
                return dev_write(self.path, data, len(data), addr)

        # Write an array to a device.
        # @param ary Array to write.
        # @param flt Set \p flt to true to indicate that ary contains floats.
        # @param addr Address to start writing at.
        def write_list(self, ary, flt = False, addr = None):
                return dev_write_list(self.path, ary, len(ary), flt, addr)

        # Read an array from a device.
        # @param num Number of elements to read.
        # @param flt Set \p flt to true to indicate that the read data are floats.
        # @param addr Address to start reading from.
        # @return The array list read from
        def read_list(self, num, flt = False, addr = None):
                return dev_read_list(self.path, num, flt, addr)

        ## Write a numeral character to a device.
        # @param num Integer or float to write.
        # @param addr Address to write to.
        # @return An error code.
        def write_numeral(self, num, addr = None):
                return dev_write_numeral(self.path, num, addr)

        ## Read a numberal from a device.
        # @param addr Address to read from.
        # @param flt Set \p flt to true to read the input as a float.
        # @return The numeral read from the device.
        def read_numeral(self, addr = None, flt = False):
                return dev_read_numeral(self.path, flt, addr)

## @}

def dev_write_numeral(desc, num, addr):
        """__NATIVE__
        pPmObj_t desc, numeral, address;
        const uint8_t *fd;
        size_t addr, size;
        const void *buf;

        desc = NATIVE_GET_LOCAL(0);
        numeral = NATIVE_GET_LOCAL(1);
        address = NATIVE_GET_LOCAL(2);

        if(OBJ_GET_TYPE(numeral) == OBJ_TYPE_FLT) {
                buf = &((pPmFloat_t)numeral)->val;
                size = sizeof(float);
        } else {
                buf = &((pPmInt_t)numeral)->val;
                size = sizeof(int32_t);
        }

        fd = ((pPmString_t)desc)->val;
        if(address != PM_NONE) {
                addr = ((pPmInt_t)address)->val;
                device_write(fd, buf, size, &addr);
        } else {
                device_write(fd, buf, size, NULL);
        }

        NATIVE_SET_TOS(PM_NONE);
        return PM_RET_OK;
        """
        pass

def dev_read_numeral(desc, flt, addr):
        """__NATIVE__
        pPmObj_t desc, numeral, address, flt;
        int rv;
        const uint8_t *fd;
        size_t addr, size;
        uint8_t *buf;
        float *f;
        int32_t *i;

        desc = NATIVE_GET_LOCAL(0);
        flt = NATIVE_GET_LOCAL(1);
        address = NATIVE_GET_LOCAL(2);

        fd = ((pPmString_t)desc)->val;
        size = (flt == PM_TRUE) ? sizeof(float) : sizeof(int32_t);
        buf = kzalloc(size);
        if(address != PM_NONE) {
                addr = ((pPmInt_t)address)->val;
                rv = device_read(fd, buf, size, &addr);
        } else {
                rv = device_read(fd, buf, size, NULL);
        }

        if(rv != -EOK) {
                kfree(buf);
                NATIVE_SET_TOS(PM_NONE);
                return PM_RET_OK;
        }

        if(flt == PM_TRUE) {
                f = (float*)buf;
                float_new(*f, &numeral);
        } else {
                i = (int32_t*)buf;
                int_new(*i, &numeral);
        }

        kfree(buf);
        NATIVE_SET_TOS(numeral);
        return PM_RET_OK;
        """
        pass

def dev_read_list(desc, num, flt, addr):
        """__NATIVE__
        pPmObj_t pdesc, pary, plength, paddr, obj, pflt;
        int idx, rv;
        const uint8_t *fd;
        size_t address, length, size;
        float *farry;
        int32_t *iarry;
        void *buf;

        pdesc = NATIVE_GET_LOCAL(0);
        plength = NATIVE_GET_LOCAL(1);
        pflt = NATIVE_GET_LOCAL(2);
        paddr = NATIVE_GET_LOCAL(3);

        fd = ((pPmString_t)pdesc)->val;
        length = ((pPmInt_t)plength)->val;
        list_new(&pary);

        if(pflt == PM_TRUE) {
                size = length * sizeof(float);
        } else {
                size = length * sizeof(int32_t);
        }

        buf = kzalloc(size);
        farry = buf;
        iarry = buf;

        if(paddr != PM_NONE) {
                address = ((pPmInt_t)paddr)->val;
                rv = device_read(fd, buf, size, &address);
        } else {
                rv = device_read(fd, buf, size, NULL);
        }

        if(rv != -EOK) {
                kfree(buf);
                NATIVE_SET_TOS(pary);
                return PM_RET_OK;
        }

        for(idx = 0; idx < length; idx++) {
                pflt == PM_TRUE ? float_new(farry[idx], &obj) :
                                  int_new(iarry[idx], &obj);
                list_append(pary, obj);
        }

        kfree(buf);
        NATIVE_SET_TOS(pary);
        return PM_RET_OK;
        """
        pass

def dev_write_list(desc, ary, length, flt, addr):
        """__NATIVE__
        pPmObj_t pdesc, pary, plength, paddr, obj, retval, pflt;
        int idx, rv;
        const uint8_t *fd;
        size_t address, length;
        float *farry;
        int32_t *iarry;
        const void *buf;

        pdesc = NATIVE_GET_LOCAL(0);
        pary = NATIVE_GET_LOCAL(1);
        plength = NATIVE_GET_LOCAL(2);
        pflt = NATIVE_GET_LOCAL(3);
        paddr = NATIVE_GET_LOCAL(4);

        fd = ((pPmString_t)pdesc)->val;
        length = ((pPmInt_t)plength)->val;

        if(pflt == PM_TRUE) {
                farry = kzalloc(length * sizeof(float));
                for(idx = 0; idx < length; idx++) {
                        list_getItem(pary, idx, &obj);
                        farry[idx] = ((pPmFloat_t)obj)->val;
                }

                length *= sizeof(float);
                buf = farry;
        } else {
                iarry = kzalloc(length * sizeof(int32_t));
                for(idx = 0; idx < length; idx++) {
                        list_getItem(pary, idx, &obj);
                        iarry[idx] = ((pPmInt_t)obj)->val;
                }

                length *= sizeof(int32_t);
                buf = iarry;
        }

        if(paddr != PM_NONE) {
                address = ((pPmInt_t)paddr)->val;
                rv = device_write(fd, buf, length, &address);
        } else {
                rv = device_write(fd, buf, length, NULL);
        }

        kfree((void*)buf);
        int_new(rv, &retval);
        NATIVE_SET_TOS(retval);
        return PM_RET_OK;
        """
        pass

def dev_write(desc, data, num, addr):
        """__NATIVE__
        pPmObj_t desc, data, num, addr, retval;
        int rv;
        const uint8_t *fd;
        void *buf;
        size_t address, nbytes;

        desc = NATIVE_GET_LOCAL(0);
        data = NATIVE_GET_LOCAL(1);
        num = NATIVE_GET_LOCAL(2);
        addr = NATIVE_GET_LOCAL(3);

        fd = ((pPmString_t)desc)->val;
        buf = ((pPmString_t)data)->val;
        nbytes = ((pPmInt_t)num)->val + 1;

        if(addr != PM_NONE) {
                address = ((pPmInt_t)addr)->val;
                rv = device_write(fd, buf, nbytes, &address);
        } else {
                rv = device_write(fd, buf, nbytes, NULL);
        }

        int_new(rv, &retval);
        NATIVE_SET_TOS(retval);
        return PM_RET_OK;
        """
        pass

def dev_read(desc, num, addr):
        """__NATIVE__
        pPmObj_t desc, data, num, addr;
        int rv;
        const uint8_t *fd;
        char *buf;
        size_t address, nbytes;

        desc = NATIVE_GET_LOCAL(0);
        num = NATIVE_GET_LOCAL(1);
        addr = NATIVE_GET_LOCAL(2);

        fd = ((pPmString_t)desc)->val;
        nbytes = ((pPmInt_t)num)->val;
        buf = kzalloc(nbytes + 1);

        if(addr != PM_NONE) {
                address = ((pPmInt_t)addr)->val;
                rv = device_read(fd, buf, nbytes, &address);
        } else {
                rv = device_read(fd, buf, nbytes, NULL);
        }

        if(rv != -EOK) {
                NATIVE_SET_TOS(PM_NONE);
                kfree(buf);
                return PM_RET_OK;
        }

        string_new((const char*)&buf, &data);
        kfree(buf);
        NATIVE_SET_TOS(data);
        return PM_RET_OK;
        """
        pass
