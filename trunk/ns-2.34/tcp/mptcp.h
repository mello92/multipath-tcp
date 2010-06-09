/*
 * Copyright (C) 2010 WIDE Project.  All rights reserved.
 *
 * Yoshifumi Nishida  <nishida@sfc.wide.ad.jp>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "tcp-full.h"
#include "mptcp-full.h"
#include "agent.h"
#include "node.h"
#include "packet.h"
#include <vector>

#define MAX_SUBFLOW 100

struct subflow
{
  subflow ():used (false), addr_ (0), port_ (0), daddr_ (-1), dport_ (-1),
    link_ (NULL), target_ (NULL), tcp_ (NULL), scwnd_ (0)
  {
  };
  bool used;
  int addr_;
  int port_;
  int daddr_;
  int dport_;
  NsObject *link_;
  NsObject *target_;
  MpFullTcpAgent *tcp_;
  int dstid_;
  double scwnd_;
};

struct dstinfo
{
  dstinfo ():addr_ (-1), port_ (-1), active_ (false)
  {
  };
  int addr_;
  int port_;
  bool active_;
};

struct dack_mapping
{
  int ackno;
  int length;
};

class MptcpAgent:public Agent
{
  friend class XcpEndsys;
  virtual void sendmsg (int nbytes, const char *flags = 0);
public:
    MptcpAgent ();
   ~MptcpAgent ()
  {
  };
  void delay_bind_init_all ();
  void recv (Packet * pkt, Handler *);
  void set_dataack (int ackno, int length);
  int get_dataack ()
  {
    return mackno_;
  }
  double get_alpha ()
  {
    return alpha_;
  }
  int get_totalcwnd ()
  {
    return totalcwnd_;
  }
  int command (int argc, const char *const *argv);
  TracedInt curseq_;
protected:
  int get_subnum ();
  int find_subflow (int addr, int port);
  int find_subflow (int addr);
  void calc_alpha ();
  void send_control ();
  void add_destination (int addr, int port);
  void calculate_alpha ();
  bool check_routable (int sid, int addr, int port);

  Classifier *core_;
  int sub_num_;
  int dst_num_;
  int total_bytes_;
  int mcurseq_;
  int mackno_;
  int totalcwnd_;
  double alpha_;
  struct subflow subflows_[MAX_SUBFLOW];
  struct dstinfo dsts_[MAX_SUBFLOW];
  vector < dack_mapping > dackmap_;
};
