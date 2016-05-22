<?php

namespace Drupal\redhen\ParamConverter;

use Drupal\Core\ParamConverter\ParamConverterInterface;
use Symfony\Component\Routing\Route;

class RedhenParamConverter implements ParamConverterInterface {
  public function convert($value, $definition, $name, array $defaults) {
    $redhen_types = ['org' => 'redhen_org', 'contact' => 'redhen_contact'];
    return $redhen_types[$value];
  }

  public function applies($definition, $name, Route $route) {
    return (!empty($definition['type']) && $definition['type'] == 'redhen_type');
  }
}