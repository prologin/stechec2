def camel_case(value: str) -> str:
    """Convert a snake case identifier to a upper camel case one"""
    return "".join(i.capitalize() for i in value.split("_"))
