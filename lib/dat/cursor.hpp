/* -*- c-basic-offset: 2 -*- */
/* Copyright(C) 2011 Brazil

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License version 2.1 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef GRN_DAT_CURSOR_HPP_
#define GRN_DAT_CURSOR_HPP_

#include "key.hpp"

namespace grn {
namespace dat {

class Cursor {
 public:
  Cursor() {}
  virtual ~Cursor() {}

  virtual void close() = 0;

  virtual const Key &next() = 0;

  virtual UInt32 offset() const = 0;
  virtual UInt32 limit() const = 0;
  virtual UInt32 flags() const = 0;

 private:
  // Disallows copy and assignment.
  Cursor(const Cursor &);
  Cursor &operator=(const Cursor &);
};

}  // namespace dat
}  // namespace grn

#endif  // GRN_DAT_CURSOR_HPP_
