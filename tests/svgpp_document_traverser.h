#pragma once

/**
 * @brief Traverse an SVG document unsing SVG++
 * The SvgppDocumentTraverser class traverses the SVG document through
 * the SVG++ document loader and provides it with a context containing callbacks
 * for handling of SVG elements.
 */

class ISvgppContext;

class SvgppDocumentTraverser {
public:
  void traverseSvg(ISvgppContext &context, char *text);
};
