示例1: serial_term_send_msg
/*
 *	Send message to serial port
 */
static int
serial_term_send_msg(struct ipmi_intf * intf, uint8_t * msg, int msg_len)
{
    int i, size, tmp = 0;
    uint8_t * buf, * data;//无符号8位整型

    if (verbose > 3) {
        fprintf(stderr, "Sending request:\n");
        fprintf(stderr, "  NetFN/rsLUN  = 0x%x\n", msg[0]);
        fprintf(stderr, "  rqSeq        = 0x%x\n", msg[1]);
        fprintf(stderr, "  cmd          = 0x%x\n", msg[2]);
        if (msg_len > 7) {
            fprintf(stderr, "  data_len     = %d\n", msg_len - 3);//buff长度-3 
            fprintf(stderr, "  data         = %s\n",//打印data部分 也就是 msgraw位3位后
                    buf2str(msg + 3, msg_len - 3));//buf2str msg3位后所有 传入一个msg的长度
        }//格式buf2str example: buf2str(0x2a)  -> "\u002a"
    }

    if (verbose > 4) {
        fprintf(stderr, "Message data:\n");
        fprintf(stderr, " %s\n", buf2str(msg, msg_len));
    }

    /* calculate required buffer size */
    size = msg_len * 2 + 4;//计算长度

    /* allocate buffer for output data */
    buf = data = (uint8_t *) alloca(size);//alloca (size,sizeof(uint8_t *))??

    if (!buf) {
        lperror(LOG_ERR, "ipmitool: alloca error");//报错分配内存出错
        return -1;
    }

    /* start character */
    *buf++ = '[';//给buff第一位赋一个[表示开始

    /* body */
    for (i = 0; i < msg_len; i++) {
        buf += sprintf( buf, "%02x", msg[i]);//首先把msg[I]的数据传入buff
 // 并且让buf指针 +=2（msg[i]的的长度）使buff指向下一区域 比如0x10 0x20 buff输入0x10数据 buff[0]=1 [1]=0
 //( buf, "%02x", msg[i]) 返回2 buf+=2 使指针指向buff[2] 执行下一次循环
    }

    /* stop character */
    *buf++ = ']';

    /* carriage return */
    *buf++ = '\r';

    /* line feed */
    *buf++ = '\n';//最终buf指针指向\n的下一个内存

    /* write data to serial port */
    tmp = write(intf->fd, data, size);//data指向分配内存的第一位 返回一个data输入字节数interger
    if (tmp <= 0) {
      //tmp无数据表明写入错误
        lperror(LOG_ERR, "ipmitool: write error");
        return -1;
    }

    return 0;
}
开发者ID:BenTech2，项目名称:ipmitool，代码行数:63，代码来源:serial_terminal.c


示例2: add_header
//添加头
static int add_header(grpc_http_parser *parser) {
  uint8_t *beg = parser->cur_line;
  uint8_t *cur = beg;
  uint8_t *end = beg + parser->cur_line_length;
  size_t *hdr_count = NULL;
  grpc_http_header **hdrs = NULL;
  grpc_http_header hdr = {NULL, NULL};

  GPR_ASSERT(cur != end);

  if (*cur == ' ' || *cur == '\t') {
    if (grpc_http1_trace)
      gpr_log(GPR_ERROR, "Continued header lines not supported yet");
    goto error;
  }

  while (cur != end && *cur != ':') {
    cur++;
  }
  if (cur == end) {
    if (grpc_http1_trace) {
      gpr_log(GPR_ERROR, "Didn't find ':' in header string");
    }
    goto error;
  }
  GPR_ASSERT(cur >= beg);
  hdr.key = buf2str(beg, (size_t)(cur - beg));
  cur++; /* skip : */

  while (cur != end && (*cur == ' ' || *cur == '\t')) {
    cur++;
  }
  GPR_ASSERT((size_t)(end - cur) >= parser->cur_line_end_length);
  hdr.value = buf2str(cur, (size_t)(end - cur) - parser->cur_line_end_length);

  if (parser->type == GRPC_HTTP_RESPONSE) {
    hdr_count = &parser->http.response.hdr_count;
    hdrs = &parser->http.response.hdrs;
  } else if (parser->type == GRPC_HTTP_REQUEST) {
    hdr_count = &parser->http.request.hdr_count;
    hdrs = &parser->http.request.hdrs;
  } else {
    return 0;
  }

  if (*hdr_count == parser->hdr_capacity) {
    parser->hdr_capacity =
        GPR_MAX(parser->hdr_capacity + 1, parser->hdr_capacity * 3 / 2);
    *hdrs = gpr_realloc(*hdrs, parser->hdr_capacity * sizeof(**hdrs));
  }
  (*hdrs)[(*hdr_count)++] = hdr;
  return 1;

error:
  gpr_free(hdr.key);
  gpr_free(hdr.value);
  return 0;
}
开发者ID:201528013359030，项目名称:grpc，代码行数:58，代码来源:parser.c

示例3: serial_bm_recv_msg
▲ 点赞 5 ▼
/*
 *	Read and parse data from serial port
 */
static int
serial_bm_recv_msg(struct ipmi_intf * intf,//ipmiintf结构体指针
        struct serial_bm_recv_ctx * recv_ctx,//收到消息结构体指针
        uint8_t * msg_data, size_t msg_len)//data 数据 msglen 长度
{
    struct serial_bm_parse_ctx parse_ctx;
    int rv;

    parse_ctx.state = MSG_NONE;
    parse_ctx.msg = msg_data;
    parse_ctx.max_len = msg_len;

    do {
        /* wait for data in the port */
        if (serial_bm_wait_for_data(intf)) {
            return 0;
        }

        /* read data into buffer */
        rv = read(intf->fd, recv_ctx->buffer + recv_ctx->buffer_size,
                recv_ctx->max_buffer_size - recv_ctx->buffer_size);

        if (rv < 0) {
            lperror(LOG_ERR, "ipmitool: read error");
            return -1;
        }

        if (verbose > 5) {
            fprintf(stderr, "Received serial data:\n %s\n",
                    buf2str(recv_ctx->buffer + recv_ctx->buffer_size, rv));
        }

        /* increment buffer size */
        recv_ctx->buffer_size += rv;

        /* parse buffer */
        rv = serial_bm_parse_buffer(recv_ctx->buffer,
                recv_ctx->buffer_size, &parse_ctx);

        if (rv < recv_ctx->buffer_size) {
            /* move non-parsed part of the buffer to the beginning */
            memmove(recv_ctx->buffer, recv_ctx->buffer + rv,
                    recv_ctx->buffer_size - rv);
        }

        /* decrement buffer size */
        recv_ctx->buffer_size -= rv;
    } while (parse_ctx.state != MSG_DONE);

    if (verbose > 4) {
        printf("Received message:\n %s\n",
                buf2str(msg_data, parse_ctx.msg_len));
    }

    /* received a message */
    return parse_ctx.msg_len;
}
开发者ID:BenTech2，项目名称:ipmitool，代码行数:60，代码来源:serial_basic.c


示例4: handle_request_line
▲ 点赞 2 ▼
static int handle_request_line(grpc_http_parser *parser) {
  uint8_t *beg = parser->cur_line;
  uint8_t *cur = beg;
  uint8_t *end = beg + parser->cur_line_length;
  uint8_t vers_major = 0;
  uint8_t vers_minor = 0;

  while (cur != end && *cur++ != ' ')
    ;
  if (cur == end) goto error;
  parser->http.request.method = buf2str(beg, (size_t)(cur - beg - 1));

  beg = cur;
  while (cur != end && *cur++ != ' ')
    ;
  if (cur == end) goto error;
  parser->http.request.path = buf2str(beg, (size_t)(cur - beg - 1));

  if (cur == end || *cur++ != 'H') goto error;
  if (cur == end || *cur++ != 'T') goto error;
  if (cur == end || *cur++ != 'T') goto error;
  if (cur == end || *cur++ != 'P') goto error;
  if (cur == end || *cur++ != '/') goto error;
  vers_major = (uint8_t)(*cur++ - '1' + 1);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
  ++cur;
  if (cur == end) goto error;
  vers_minor = (uint8_t)(*cur++ - '1' + 1);

  if (vers_major == 1) {
    if (vers_minor == 0) {
      parser->http.request.version = GRPC_HTTP_HTTP10;
    } else if (vers_minor == 1) {
      parser->http.request.version = GRPC_HTTP_HTTP11;
    } else {
      goto error;
    }
  } else if (vers_major == 2) {
    if (vers_minor == 0) {
      parser->http.request.version = GRPC_HTTP_HTTP20;
    } else {
      goto error;
    }
  } else {
    goto error;
  }

  return 1;

error:
  if (grpc_http1_trace) gpr_log(GPR_ERROR, "Failed parsing request line");
  return 0;
}
开发者ID:BridgeFi，项目名称:ParkingTicketTracker，代码行数:52，代码来源:parser.c

示例5: rndr_tablecell
▲ 点赞 1 ▼
static void
rndr_tablecell(struct buf *ob, const struct buf *text, int align, void *opaque)
{
    VALUE rb_align;

    switch (align) {
    case MKD_TABLE_ALIGN_L:
        rb_align = CSTR2SYM("left");
        break;

    case MKD_TABLE_ALIGN_R:
        rb_align = CSTR2SYM("right");
        break;

    case MKD_TABLE_ALIGN_CENTER:
        rb_align = CSTR2SYM("center");
        break;

    default:
        rb_align = Qnil;
        break;
    }

    BLOCK_CALLBACK("table_cell", 2, buf2str(text), rb_align);
}
开发者ID:SachaG，项目名称:redcarpet，代码行数:25，代码来源:rc_render.c


示例6: ipmi_auth_md5
▲ 点赞 1 ▼
/*
 * multi-session authcode generation for MD5
 * H(password + session_id + msg + session_seq + password)
 *
 * Use OpenSSL implementation of MD5 algorithm if found
 */
uint8_t * ipmi_auth_md5(struct ipmi_session * s, uint8_t * data, int data_len)
{
    md5_state_t state;
    static md5_byte_t digest[16];
    uint32_t temp;

    memset(digest, 0, 16);
    memset(&state, 0, sizeof(md5_state_t));

    md5_init(&state);

    md5_append(&state, (const md5_byte_t *)s->authcode, 16);
    md5_append(&state, (const md5_byte_t *)&s->session_id, 4);
    md5_append(&state, (const md5_byte_t *)data, data_len);

        if(!isLittleEndian())
     temp = BSWAP_32(s->in_seq);
        else
     temp = s->in_seq;

    md5_append(&state, (const md5_byte_t *)&temp, 4);
    md5_append(&state, (const md5_byte_t *)s->authcode, 16);

    md5_finish(&state, digest);

    if (verbose > 3)
        printf("  MD5 AuthCode    : %s\n", buf2str(digest, 16));
    return digest;
}
开发者ID:adiantum，项目名称:NMPRK，代码行数:35，代码来源:auth.c

示例7: serial_term_wait_response
▲ 点赞 1 ▼
/*
 *	Wait for request response
 */
static int
serial_term_wait_response(struct ipmi_intf * intf,
        struct serial_term_request_ctx * req_ctx,
        uint8_t * msg, size_t max_len)
{
    struct serial_term_hdr * hdr = (struct serial_term_hdr *) msg;
    int msg_len;

    /* wait for response(s) */
    do {
        /* receive message */
        msg_len = recv_response(intf, msg, max_len);

        /* check if valid message received  */
        if (msg_len > 0) {
            /* validate message size */
            if (msg_len < 4) {
                /* either bad response or non-related message */
                continue;
            }

            /* check for the waited response */
            if (hdr->netFn == (req_ctx->netFn|4)
                    && (hdr->seq & ~3) == req_ctx->seq
                    && hdr->cmd == req_ctx->cmd) {
                /* check if something new has been parsed */
                if (verbose > 3) {
                    fprintf(stderr, "Got response:\n");
                    fprintf(stderr, "  NetFN/rsLUN     = 0x%x\n", msg[0]);
                    fprintf(stderr, "  rqSeq/Bridge    = 0x%x\n", msg[1]);
                    fprintf(stderr, "  cmd             = 0x%x\n", msg[2]);
                    fprintf(stderr, "  completion code = 0x%x\n", msg[3]);
                    if (msg_len > 8) {
                        fprintf(stderr, "  data_len        = %d\n",
                                msg_len - 4);
                        fprintf(stderr, "  data            = %s\n",
                                buf2str(msg + 4, msg_len - 4));
                    }
                }

                /* move to start from completion code */
                memmove(msg, hdr + 1, msg_len - sizeof (*hdr));

                /* the waited one */
                return msg_len - sizeof (*hdr);
            }
        }
    } while (msg_len > 0);

    return 0;
}
开发者ID:BenTech2，项目名称:ipmitool，代码行数:54，代码来源:serial_terminal.c


示例8: impl
▲ 点赞 1 ▼
size_t LLXMLRPCTransaction::Impl::curlDownloadCallback(
        char* data, size_t size, size_t nmemb, void* user_data)
{
    Impl& impl(*(Impl*)user_data);
    
    size_t n = size * nmemb;

#ifdef CWDEBUG
    if (n < 80)
      Dout(dc::curl, "Entering LLXMLRPCTransaction::Impl::curlDownloadCallback(\"" << buf2str(data, n) << "\", " << size << ", " << nmemb << ", " << user_data << ")");
    else
      Dout(dc::curl, "Entering LLXMLRPCTransaction::Impl::curlDownloadCallback(\"" << buf2str(data, 40) << "\"...\"" << buf2str(data + n - 40, 40) << "\", " << size << ", " << nmemb << ", " << user_data << ")");
#endif

    impl.mResponseText.append(data, n);
    
    if (impl.mStatus == LLXMLRPCTransaction::StatusStarted)
    {
        impl.setStatus(LLXMLRPCTransaction::StatusDownloading);
    }
    
    return n;
}
开发者ID:samiam123，项目名称:VoodooNxg，代码行数:23，代码来源:llxmlrpctransaction.cpp

示例9: rndr_paragraph
▲ 点赞 1 ▼
static void
rndr_paragraph(struct buf *ob, const struct buf *text, void *opaque)
{
    BLOCK_CALLBACK("paragraph", 1, buf2str(text));
}
开发者ID:SachaG，项目名称:redcarpet，代码行数:5，代码来源:rc_render.c

示例10: rndr_listitem
▲ 点赞 1 ▼
static void
rndr_listitem(struct buf *ob, const struct buf *text, int flags, void *opaque)
{
    BLOCK_CALLBACK("list_item", 2, buf2str(text),
            (flags & MKD_LIST_ORDERED) ? CSTR2SYM("ordered") : CSTR2SYM("unordered"));
}
开发者ID:SachaG，项目名称:redcarpet，代码行数:6，代码来源:rc_render.c

示例11: rndr_header
▲ 点赞 1 ▼
static void
rndr_header(struct buf *ob, const struct buf *text, int level, void *opaque)
{
    BLOCK_CALLBACK("header", 2, buf2str(text), INT2FIX(level));
}
开发者ID:SachaG，项目名称:redcarpet，代码行数:5，代码来源:rc_render.c

示例12: rndr_codespan
▲ 点赞 1 ▼
static int
rndr_codespan(struct buf *ob, const struct buf *text, void *opaque)
{
    SPAN_CALLBACK("codespan", 1, buf2str(text));
}
开发者ID:SachaG，项目名称:redcarpet，代码行数:5，代码来源:rc_render.c

示例13: ipmi_openipmi_send_cmd
▲ 点赞 1 ▼
static struct ipmi_rs *
ipmi_openipmi_send_cmd(struct ipmi_intf * intf, struct ipmi_rq * req)
{
    struct ipmi_recv recv;
    struct ipmi_addr addr;
    struct ipmi_system_interface_addr bmc_addr = {
        .addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE,
        .channel = IPMI_BMC_CHANNEL,
    };
    struct ipmi_ipmb_addr ipmb_addr = {
        .addr_type = IPMI_IPMB_ADDR_TYPE,
    };
    struct ipmi_req _req;
    static struct ipmi_rs rsp;
    struct timeval read_timeout;
    static int curr_seq = 0;
    fd_set rset;

    uint8_t * data = NULL;
    int data_len = 0;
    int retval = 0;


    if (intf == NULL || req == NULL)
        return NULL;

    ipmb_addr.channel = intf->target_channel & 0x0f;

    if (intf->opened == 0 && intf->open != NULL)
        if (intf->open(intf) < 0)
            return NULL;

    if (verbose > 2) {
        fprintf(stderr, "OpenIPMI Request Message Header:\n");
        fprintf(stderr, "  netfn     = 0x%x\n",  req->msg.netfn );
        fprintf(stderr, "  cmd       = 0x%x\n", req->msg.cmd);
        printbuf(req->msg.data, req->msg.data_len, "OpenIPMI Request Message Data");
    }
        


    /*
     * setup and send message
     */

    memset(&_req, 0, sizeof(struct ipmi_req));

    if (intf->target_addr != 0 &&
        intf->target_addr != intf->my_addr) {
        /* use IPMB address if needed */
        ipmb_addr.slave_addr = intf->target_addr;
        ipmb_addr.lun = req->msg.lun;
        lprintf(LOG_DEBUG, "Sending request 0x%x to "
            "IPMB target @ 0x%x:0x%x (from 0x%x)", 
            req->msg.cmd,
            intf->target_addr,intf->target_channel, intf->my_addr);

        if(intf->transit_addr != 0 && intf->transit_addr != intf->my_addr) { 
           uint8_t index = 0;
      
           lprintf(LOG_DEBUG, "Encapsulating data sent to "
               "end target [0x%02x,0x%02x] using transit [0x%02x,0x%02x] from 0x%x ",
               (0x40 | intf->target_channel),
               intf->target_addr,
               intf->transit_channel,
               intf->transit_addr,
               intf->my_addr
               );      

           /* Convert Message to 'Send Message' */
           /* Supplied req : req , internal req : _req  */

           if (verbose > 4) {
              fprintf(stderr, "Converting message:\n");
              fprintf(stderr, "  netfn     = 0x%x\n",  req->msg.netfn );
              fprintf(stderr, "  cmd       = 0x%x\n", req->msg.cmd);
              if (req->msg.data && req->msg.data_len) {
             fprintf(stderr, "  data_len  = %d\n", req->msg.data_len);
             fprintf(stderr, "  data      = %s\n",
                 buf2str(req->msg.data,req->msg.data_len));
              }
           }

           /* Modify target address to use 'transit' instead */
           ipmb_addr.slave_addr = intf->transit_addr;
           ipmb_addr.channel    = intf->transit_channel;

           /* FIXME backup "My address" */
           data_len = req->msg.data_len + 8;
           data = malloc(data_len);
           if (data == NULL) {
              lprintf(LOG_ERR, "ipmitool: malloc failure");
              return NULL;
           }

           memset(data, 0, data_len);

           data[index++] = (0x40|intf->target_channel);
           data[index++] = intf->target_addr;
           data[index++] = (  req->msg.netfn << 2 ) |  req->msg.lun ;
//.........这里部分代码省略.........
开发者ID:BenTech2，项目名称:ipmitool，代码行数:101，代码来源:open.c

示例14: rndr_quote
▲ 点赞 1 ▼
static int
rndr_quote(struct buf *ob, const struct buf *text, void *opaque)
{
    SPAN_CALLBACK("quote", 1, buf2str(text));
}
开发者ID:Adoyle2014，项目名称:redcarpet，代码行数:5，代码来源:rc_render.c

示例15: rndr_entity
▲ 点赞 1 ▼
/**
 * direct writes
 */
static void
rndr_entity(struct buf *ob, const struct buf *text, void *opaque)
{
    BLOCK_CALLBACK("entity", 1, buf2str(text));
}