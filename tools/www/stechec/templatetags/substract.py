from django import template
register = template.Library()

@register.filter
def substract(value, arg):
    try:
        value = int(value)
    except ValueError:
        return value
    return str(value - arg)
