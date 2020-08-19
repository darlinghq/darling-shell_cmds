/*
 * This file was generated by the mknodes program.
 */

/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kenneth Almquist.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)nodes.c.pat	8.2 (Berkeley) 5/4/95
 * $FreeBSD: head/bin/sh/nodes.c.pat 314436 2017-02-28 23:42:47Z imp $
 */

#include <sys/param.h>
#include <stdlib.h>
#include <stddef.h>
/*
 * Routine for dealing with parsed shell commands.
 */

#include "shell.h"
#include "nodes.h"
#include "memalloc.h"
#include "mystring.h"


struct nodesize {
	int     blocksize;	/* size of structures in function */
	int     stringsize;	/* size of strings in node */
};

struct nodecopystate {
	pointer block;		/* block to allocate function from */
	char   *string;		/* block to allocate strings from */
};

static const short nodesize[27] = {
      ALIGN(sizeof (struct nbinary)),
      ALIGN(sizeof (struct ncmd)),
      ALIGN(sizeof (struct npipe)),
      ALIGN(sizeof (struct nredir)),
      ALIGN(sizeof (struct nredir)),
      ALIGN(sizeof (struct nredir)),
      ALIGN(sizeof (struct nbinary)),
      ALIGN(sizeof (struct nbinary)),
      ALIGN(sizeof (struct nif)),
      ALIGN(sizeof (struct nbinary)),
      ALIGN(sizeof (struct nbinary)),
      ALIGN(sizeof (struct nfor)),
      ALIGN(sizeof (struct ncase)),
      ALIGN(sizeof (struct nclist)),
      ALIGN(sizeof (struct nclist)),
      ALIGN(sizeof (struct narg)),
      ALIGN(sizeof (struct narg)),
      ALIGN(sizeof (struct nfile)),
      ALIGN(sizeof (struct nfile)),
      ALIGN(sizeof (struct nfile)),
      ALIGN(sizeof (struct nfile)),
      ALIGN(sizeof (struct nfile)),
      ALIGN(sizeof (struct ndup)),
      ALIGN(sizeof (struct ndup)),
      ALIGN(sizeof (struct nhere)),
      ALIGN(sizeof (struct nhere)),
      ALIGN(sizeof (struct nnot)),
};


static void calcsize(union node *, struct nodesize *);
static void sizenodelist(struct nodelist *, struct nodesize *);
static union node *copynode(union node *, struct nodecopystate *);
static struct nodelist *copynodelist(struct nodelist *, struct nodecopystate *);
static char *nodesavestr(const char *, struct nodecopystate *);


struct funcdef {
	unsigned int refcount;
	union node n;
};

/*
 * Make a copy of a parse tree.
 */

struct funcdef *
copyfunc(union node *n)
{
	struct nodesize sz;
	struct nodecopystate st;
	struct funcdef *fn;

	if (n == NULL)
		return NULL;
	sz.blocksize = offsetof(struct funcdef, n);
	sz.stringsize = 0;
	calcsize(n, &sz);
	fn = ckmalloc(sz.blocksize + sz.stringsize);
	fn->refcount = 1;
	st.block = (char *)fn + offsetof(struct funcdef, n);
	st.string = (char *)fn + sz.blocksize;
	copynode(n, &st);
	return fn;
}


union node *
getfuncnode(struct funcdef *fn)
{
	return fn == NULL ? NULL : &fn->n;
}


static void
calcsize(union node *n, struct nodesize *result)
{
      if (n == NULL)
	    return;
      result->blocksize += nodesize[n->type];
      switch (n->type) {
      case NSEMI:
      case NAND:
      case NOR:
      case NWHILE:
      case NUNTIL:
	    calcsize(n->nbinary.ch2, result);
	    calcsize(n->nbinary.ch1, result);
	    break;
      case NCMD:
	    calcsize(n->ncmd.redirect, result);
	    calcsize(n->ncmd.args, result);
	    break;
      case NPIPE:
	    sizenodelist(n->npipe.cmdlist, result);
	    break;
      case NREDIR:
      case NBACKGND:
      case NSUBSHELL:
	    calcsize(n->nredir.redirect, result);
	    calcsize(n->nredir.n, result);
	    break;
      case NIF:
	    calcsize(n->nif.elsepart, result);
	    calcsize(n->nif.ifpart, result);
	    calcsize(n->nif.test, result);
	    break;
      case NFOR:
	    result->stringsize += strlen(n->nfor.var) + 1;
	    calcsize(n->nfor.body, result);
	    calcsize(n->nfor.args, result);
	    break;
      case NCASE:
	    calcsize(n->ncase.cases, result);
	    calcsize(n->ncase.expr, result);
	    break;
      case NCLIST:
      case NCLISTFALLTHRU:
	    calcsize(n->nclist.body, result);
	    calcsize(n->nclist.pattern, result);
	    calcsize(n->nclist.next, result);
	    break;
      case NDEFUN:
      case NARG:
	    sizenodelist(n->narg.backquote, result);
	    result->stringsize += strlen(n->narg.text) + 1;
	    calcsize(n->narg.next, result);
	    break;
      case NTO:
      case NFROM:
      case NFROMTO:
      case NAPPEND:
      case NCLOBBER:
	    calcsize(n->nfile.fname, result);
	    calcsize(n->nfile.next, result);
	    break;
      case NTOFD:
      case NFROMFD:
	    calcsize(n->ndup.vname, result);
	    calcsize(n->ndup.next, result);
	    break;
      case NHERE:
      case NXHERE:
	    calcsize(n->nhere.doc, result);
	    calcsize(n->nhere.next, result);
	    break;
      case NNOT:
	    calcsize(n->nnot.com, result);
	    break;
      };
}



static void
sizenodelist(struct nodelist *lp, struct nodesize *result)
{
	while (lp) {
		result->blocksize += ALIGN(sizeof(struct nodelist));
		calcsize(lp->n, result);
		lp = lp->next;
	}
}



static union node *
copynode(union node *n, struct nodecopystate *state)
{
	union node *new;

      if (n == NULL)
	    return NULL;
      new = state->block;
      state->block = (char *)state->block + nodesize[n->type];
      switch (n->type) {
      case NSEMI:
      case NAND:
      case NOR:
      case NWHILE:
      case NUNTIL:
	    new->nbinary.ch2 = copynode(n->nbinary.ch2, state);
	    new->nbinary.ch1 = copynode(n->nbinary.ch1, state);
	    break;
      case NCMD:
	    new->ncmd.redirect = copynode(n->ncmd.redirect, state);
	    new->ncmd.args = copynode(n->ncmd.args, state);
	    break;
      case NPIPE:
	    new->npipe.cmdlist = copynodelist(n->npipe.cmdlist, state);
	    new->npipe.backgnd = n->npipe.backgnd;
	    break;
      case NREDIR:
      case NBACKGND:
      case NSUBSHELL:
	    new->nredir.redirect = copynode(n->nredir.redirect, state);
	    new->nredir.n = copynode(n->nredir.n, state);
	    break;
      case NIF:
	    new->nif.elsepart = copynode(n->nif.elsepart, state);
	    new->nif.ifpart = copynode(n->nif.ifpart, state);
	    new->nif.test = copynode(n->nif.test, state);
	    break;
      case NFOR:
	    new->nfor.var = nodesavestr(n->nfor.var, state);
	    new->nfor.body = copynode(n->nfor.body, state);
	    new->nfor.args = copynode(n->nfor.args, state);
	    break;
      case NCASE:
	    new->ncase.cases = copynode(n->ncase.cases, state);
	    new->ncase.expr = copynode(n->ncase.expr, state);
	    break;
      case NCLIST:
      case NCLISTFALLTHRU:
	    new->nclist.body = copynode(n->nclist.body, state);
	    new->nclist.pattern = copynode(n->nclist.pattern, state);
	    new->nclist.next = copynode(n->nclist.next, state);
	    break;
      case NDEFUN:
      case NARG:
	    new->narg.backquote = copynodelist(n->narg.backquote, state);
	    new->narg.text = nodesavestr(n->narg.text, state);
	    new->narg.next = copynode(n->narg.next, state);
	    break;
      case NTO:
      case NFROM:
      case NFROMTO:
      case NAPPEND:
      case NCLOBBER:
	    new->nfile.fname = copynode(n->nfile.fname, state);
	    new->nfile.next = copynode(n->nfile.next, state);
	    new->nfile.fd = n->nfile.fd;
	    break;
      case NTOFD:
      case NFROMFD:
	    new->ndup.vname = copynode(n->ndup.vname, state);
	    new->ndup.dupfd = n->ndup.dupfd;
	    new->ndup.next = copynode(n->ndup.next, state);
	    new->ndup.fd = n->ndup.fd;
	    break;
      case NHERE:
      case NXHERE:
	    new->nhere.doc = copynode(n->nhere.doc, state);
	    new->nhere.next = copynode(n->nhere.next, state);
	    new->nhere.fd = n->nhere.fd;
	    break;
      case NNOT:
	    new->nnot.com = copynode(n->nnot.com, state);
	    break;
      };
      new->type = n->type;
	return new;
}


static struct nodelist *
copynodelist(struct nodelist *lp, struct nodecopystate *state)
{
	struct nodelist *start;
	struct nodelist **lpp;

	lpp = &start;
	while (lp) {
		*lpp = state->block;
		state->block = (char *)state->block +
		    ALIGN(sizeof(struct nodelist));
		(*lpp)->n = copynode(lp->n, state);
		lp = lp->next;
		lpp = &(*lpp)->next;
	}
	*lpp = NULL;
	return start;
}



static char *
nodesavestr(const char *s, struct nodecopystate *state)
{
	const char *p = s;
	char *q = state->string;
	char   *rtn = state->string;

	while ((*q++ = *p++) != '\0')
		continue;
	state->string = q;
	return rtn;
}


void
reffunc(struct funcdef *fn)
{
	if (fn)
		fn->refcount++;
}


/*
 * Decrement the reference count of a function definition, freeing it
 * if it falls to 0.
 */

void
unreffunc(struct funcdef *fn)
{
	if (fn) {
		fn->refcount--;
		if (fn->refcount > 0)
			return;
		ckfree(fn);
	}
}
