package pdfium

import (
	"errors"
	"sync"
)

// DocumentPool manages a pool of document slots for concurrent PDF processing
// This works around PDFium's threading limitations by ensuring only one render
// operation happens at a time, but allows multiple documents to be opened
type DocumentPool struct {
	size   int
	slots  chan struct{}
	mutex  sync.Mutex
	closed bool
}

// NewDocumentPool creates a pool that limits concurrent PDFium operations
// size: maximum number of concurrent render operations (recommended: 1-4)
func NewDocumentPool(size int) *DocumentPool {
	if size <= 0 {
		size = 1
	}
	pool := &DocumentPool{
		size:  size,
		slots: make(chan struct{}, size),
	}
	// Fill slots
	for i := 0; i < size; i++ {
		pool.slots <- struct{}{}
	}
	return pool
}

// Acquire gets a slot for PDF operations (blocks if pool is full)
func (p *DocumentPool) Acquire() error {
	p.mutex.Lock()
	if p.closed {
		p.mutex.Unlock()
		return errors.New("pool is closed")
	}
	p.mutex.Unlock()

	<-p.slots // Block until slot available
	return nil
}

// Release returns a slot to the pool
func (p *DocumentPool) Release() {
	select {
	case p.slots <- struct{}{}:
	default:
		// Should never happen
	}
}

// Close shuts down the pool
func (p *DocumentPool) Close() {
	p.mutex.Lock()
	defer p.mutex.Unlock()

	if !p.closed {
		p.closed = true
		close(p.slots)
	}
}

// WithSlot executes a function with an acquired slot
func (p *DocumentPool) WithSlot(fn func() error) error {
	if err := p.Acquire(); err != nil {
		return err
	}
	defer p.Release()
	return fn()
}
