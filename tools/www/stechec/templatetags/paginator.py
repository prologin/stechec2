# http://djangosnippets.org/snippets/73/

from django import template

register = template.Library()

def paginator(context, adjacent_pages=2):
    """
    To be used in conjunction with the object_list generic view.

    Adds pagination context variables for use in displaying first, adjacent and
    last page links in addition to those created by the object_list generic
    view.

    """
    page = context['page_obj']
    pagin = context['paginator']
    page_numbers = [n for n in range(page.number - adjacent_pages, page.number + adjacent_pages + 1)
                      if n > 0 and n <= pagin.num_pages]
    return {
        'page': page,
        'paginator': pagin,
        'page_numbers': page_numbers,
        'show_first': 1 not in page_numbers,
        'show_last': pagin.num_pages not in page_numbers,
    }

register.inclusion_tag('paginator.html', takes_context=True)(paginator)
