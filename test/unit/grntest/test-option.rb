# -*- coding: utf-8 -*-
#
# Copyright (C) 2011  Kouhei Sutou <kou@clear-code.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License version 2.1 as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

class GrntestOptionTest < Test::Unit::TestCase
  include GroongaGrntestTestUtils

  def setup
    setup_database
  end

  def teardown
    teardown_database
  end

  def test_log_path
    command = 'status'
    command_file = tempfile("command") do |file|
      file.puts(command)
    end
    script_file = tempfile("script") do |file|
      file.puts("do_gqtp #{command_file.path}")
    end
    log = tempfile("log")
    output, error, status = invoke_grntest("--groonga", groonga,
                                           "--protocol", "gqtp",
                                           "--log-path", log.path,
                                           script_file.path,
                                           @database_path)
    assert_predicate(status, :success?, [output, error])
    result = JSON.parse(log.read)
    assert_equal(1, result.last["queries"])
  end
end
