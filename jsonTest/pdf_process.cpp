// Created
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <iostream>

int main() {
    // Open the PDF document
    poppler::document *doc = poppler::document::load_from_file("SDTE.pdf");
    if (!doc) {
        std::cerr << "Failed to open PDF file\n";
        return 1;
    }

    // 获取PDF页数
    int numPages = doc->pages();
    if (numPages == 0) {
        std::cerr << "No pages found in the PDF file\n";
        return 1;
    }

    // Extract the first page (page numbering starts from 0)
    int pageNumber = 0;
    poppler::page *page = doc->create_page(pageNumber);
    if (!page) {
        std::cerr << "Failed to create page\n";
        delete doc;
        return 1;
    }

    // Render the page to an image
    poppler::page_renderer renderer;
    auto image = renderer.render_page(page, 100.0, 100.0); // Render at 100 DPI

    // Save the rendered image to a file (e.g., PNG)
  image.save("output.png", "png");

    // Clean up
    delete doc;
    delete page;

    return 0;
}
