<?php

namespace Drupal\redhen_contact\ContextProvider;

use Drupal\redhen_contact\Entity\Contact;
use Drupal\Core\Cache\CacheableMetadata;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\Core\Plugin\Context\ContextProviderInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Sets the current contact as context on redhen contact routes.
 *
 * @todo Remove once core gets a generic EntityRouteContext.
 */
class RedhenContactRouteContext implements ContextProviderInterface {

  use StringTranslationTrait;

  /**
   * The route match.
   *
   * @var \Drupal\Core\Routing\RouteMatchInterface
   */
  protected $routeMatch;

  /**
   * Constructs a new RedhenContactRouteContext object.
   *
   * @param \Drupal\Core\Routing\RouteMatchInterface $route_match
   *   The route match.
   */
  public function __construct(RouteMatchInterface $route_match) {
    $this->routeMatch = $route_match;
  }

  /**
   * {@inheritdoc}
   */
  public function getRuntimeContexts(array $unqualified_context_ids) {
    $context_definition = new ContextDefinition('entity:redhen_contact', NULL, FALSE);
    $value = NULL;
    if ($contact = $this->routeMatch->getParameter('redhen_contact')) {
      $value = $contact;
    }
    elseif ($this->routeMatch->getRouteName() == 'entity.redhen_contact.add_form') {
      $contact_type = $this->routeMatch->getParameter('redhen_contact_type');
      $value = Contact::create(['type' => $contact_type->id()]);
    }

    $cacheability = new CacheableMetadata();
    $cacheability->setCacheContexts(['route']);
    $context = new Context($context_definition, $value);
    $context->addCacheableDependency($cacheability);

    return ['redhen_contact' => $context];
  }

  /**
   * {@inheritdoc}
   */
  public function getAvailableContexts() {
    $context = new Context(new ContextDefinition(
      'entity:redhen_contact', $this->t('Contact from URL')
    ));
    return ['redhen_contact' => $context];
  }

}
