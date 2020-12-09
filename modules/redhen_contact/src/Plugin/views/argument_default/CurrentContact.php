<?php

namespace Drupal\redhen_contact\Plugin\views\argument_default;

use Drupal\Core\Cache\Cache;
use Drupal\Core\Cache\CacheableDependencyInterface;
use Drupal\views\Plugin\views\argument_default\ArgumentDefaultPluginBase;
use Drupal\redhen_contact\Entity\Contact;

/**
 * Default argument plugin to extract the current user's Redhen Contact.
 *
 * This plugin actually has no options so it does not need to do a great deal.
 *
 * @ViewsArgumentDefault(
 *   id = "redhen_contact_current",
 *   title = @Translation("Contact ID from logged in user's connected Contact")
 * )
 */
class CurrentContact extends ArgumentDefaultPluginBase implements CacheableDependencyInterface {

  /**
   * {@inheritdoc}
   */
  public function getArgument() {
    return Contact::loadByUser(\Drupal::currentUser())->id();
  }

  /**
   * {@inheritdoc}
   */
  public function getCacheMaxAge() {
    return Cache::PERMANENT;
  }

  /**
   * {@inheritdoc}
   */
  public function getCacheContexts() {
    return ['user'];
  }

}
