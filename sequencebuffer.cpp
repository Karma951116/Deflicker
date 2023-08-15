#include "sequencebuffer.h"


SequenceBuffer::SequenceBuffer(QObject *parent)
    : QObject{parent}
{
    sequence_ = new QList<cv::Mat>();
    index_ = 0;
}

SequenceBuffer::~SequenceBuffer()
{
    //    for (int i = 0; i < sequence->size(); i++) {
    //        sequence->at(i)->release();
    //    }
}

void SequenceBuffer::append(cv::Mat mat)
{
    sequence_->append(mat);
    if (sequence_->size() == 1) {
        pixelsChanged();
    }
}

void SequenceBuffer::clear()
{
    for (int i; i < sequence_->size(); i++) {
        cv::Mat mat = sequence_->at(i);
        if (mat.empty()) {
            continue;
        }
        mat.release();
    }
    sequence_->clear();
    indexChanged();
}

cv::Mat SequenceBuffer::read()
{
    if (index_ < 0 || index_ >= sequence_->size()) {
        return cv::Mat();
    }
    return sequence_->at(index_);
}

cv::Mat SequenceBuffer::read(int index)
{
    if (index < 0 || index >= sequence_->size()) {
        return cv::Mat();
    }
    return sequence_->at(index);
}

void SequenceBuffer::next()
{
    if (index_ < 0 || index_ >= sequence_->size()) {
        return;
    }
    if (index_ + 1 > sequence_->size() - 1) {
        return;
    }
    index_ += 1;
    indexChanged();
}

void SequenceBuffer::prev()
{
    if (index_ < 0 || index_ >= sequence_->size()) {
        return;
    }
    if (index_ - 1 < 0) {
        return;
    }
    index_ -= 1;
    indexChanged();
}

int SequenceBuffer::nbFrames()
{
    return sequence_ == nullptr ? 0 : sequence_->size();
}

int SequenceBuffer::getIndex()
{
    return index_;
}

void SequenceBuffer::insert(int index, cv::Mat mat)
{
    sequence_->insert(index, mat);
}

int SequenceBuffer::framePixels()
{
    if (sequence_->isEmpty()) {
        return 0;
    }
    cv::Mat mat = sequence_->at(0);
    return mat.rows * mat.cols;
}

void SequenceBuffer::setIndex(int index)
{
    index_ = index;
    indexChanged();
}
